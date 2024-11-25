#include <algorithm>

#include "FusionCore.h"

// Credit: Dear Imgui
#define IM_NORMALIZE2F_OVER_ZERO(VX,VY)     { float d2 = VX*VX + VY*VY; if (d2 > 0.0f) { float inv_len = ImRsqrt(d2); VX *= inv_len; VY *= inv_len; } } (void)0
#define IM_FIXNORMAL2F_MAX_INVLEN2          100.0f // 500.0f (see #4053, #3366)
#define IM_FIXNORMAL2F(VX,VY)               { float d2 = VX*VX + VY*VY; if (d2 > 0.000001f) { float inv_len2 = 1.0f / d2; if (inv_len2 > IM_FIXNORMAL2F_MAX_INVLEN2) inv_len2 = IM_FIXNORMAL2F_MAX_INVLEN2; VX *= inv_len2; VY *= inv_len2; } } (void)0


namespace CE
{
    static inline float  ImRsqrt(float x) { return 1.0f / sqrtf(x); }

    FusionRenderer2::FusionRenderer2()
    {
        circleSegmentMaxError = 0.25f;
    }

    FusionRenderer2::~FusionRenderer2()
    {
        
    }

    void FusionRenderer2::Init(const FusionRendererInitInfo& initInfo)
    {
        fusionShader = initInfo.fusionShader;
        multisampling = initInfo.multisampling;

        auto perObjectSrgLayout = fusionShader->GetDefaultVariant()->GetSrgLayout(RHI::SRGType::PerObject);
        auto perViewSrgLayout = fusionShader->GetDefaultVariant()->GetSrgLayout(RHI::SRGType::PerView);

        numFrames = RHI::FrameScheduler::Get()->GetFramesInFlight();

        perViewSrg = RHI::gDynamicRHI->CreateShaderResourceGroup(perViewSrgLayout);
        perObjectSrg = RHI::gDynamicRHI->CreateShaderResourceGroup(perObjectSrgLayout);

        objectDataBuffer.Init("ObjectData", initialObjectCount, numFrames);

        for (int i = 0; i < numFrames; ++i)
        {
            // - Create Buffers -

            RHI::BufferDescriptor viewConstantsBufferDesc{};
            viewConstantsBufferDesc.name = "ViewConstants";
            viewConstantsBufferDesc.bindFlags = RHI::BufferBindFlags::ConstantBuffer;
            viewConstantsBufferDesc.bufferSize = sizeof(RPI::PerViewConstants);
            viewConstantsBufferDesc.defaultHeapType = RHI::MemoryHeapType::Upload;
            viewConstantsBufferDesc.structureByteStride = sizeof(RPI::PerViewConstants);

            viewConstantsBuffer[i] = RHI::gDynamicRHI->CreateBuffer(viewConstantsBufferDesc);
            viewConstantsUpdateRequired[i] = true;

            RHI::BufferDescriptor quadMeshBufferDesc{};
            quadMeshBufferDesc.name = "Quad Meshes";
            quadMeshBufferDesc.bindFlags = RHI::BufferBindFlags::IndexBuffer | RHI::BufferBindFlags::VertexBuffer;
            quadMeshBufferDesc.bufferSize = initialMeshBufferSize;
            quadMeshBufferDesc.defaultHeapType = MemoryHeapType::Upload;
            quadMeshBufferDesc.structureByteStride = quadMeshBufferDesc.bufferSize;

            quadsBuffer[i] = RHI::gDynamicRHI->CreateBuffer(quadMeshBufferDesc);
            quadUpdatesRequired[i] = true;

            // - Bind Buffers -

            perViewSrg->Bind(i, "_PerViewData", viewConstantsBuffer[i]);

            perObjectSrg->Bind(i, "_Objects", objectDataBuffer.GetBuffer(i));
        }

        perViewSrg->FlushBindings();
        perObjectSrg->FlushBindings();

        for (int i = 0; i < ArcFastTableSize; i++)
        {
            const float a = ((float)i * 2 * M_PI) / (float)ArcFastTableSize;
            arcFastVertex[i] = Vec2(cosf(a), sinf(a));
        }

        arcFastRadiusCutoff = ((circleSegmentMaxError) / (1 - cosf(Math::PI / Math::Max((float)(ArcFastTableSize), Math::PI))));
    }

    void FusionRenderer2::OnBeginDestroy()
    {
	    Super::OnBeginDestroy();

        objectDataBuffer.Shutdown();

        while (freePackets.NotEmpty())
        {
            delete freePackets[0];
            freePackets.RemoveAt(0);
        }

    	for (int i = 0; i < numFrames; i++)
        {
            delete viewConstantsBuffer[i]; viewConstantsBuffer[i] = nullptr;
            delete quadsBuffer[i]; quadsBuffer[i] = nullptr;

            for (auto drawPacket : drawPacketsPerImage[i])
            {
                delete drawPacket;
            }
            drawPacketsPerImage[i].Clear();
        }

        delete perViewSrg; perViewSrg = nullptr;
        delete perObjectSrg; perObjectSrg = nullptr;
    }

    void FusionRenderer2::SetViewConstants(const RPI::PerViewConstants& viewConstants)
    {
        this->viewConstants = viewConstants;

        for (int i = 0; i < viewConstantsUpdateRequired.GetSize(); ++i)
        {
            viewConstantsUpdateRequired[i] = true;
        }
    }

    const Array<RHI::DrawPacket*>& FusionRenderer2::FlushDrawPackets(u32 imageIndex)
    {
        // - Destroy queued resources only when they are out of usage scope

        for (int i = (int)destructionQueue.GetSize() - 1; i >= 0; i--)
        {
            destructionQueue[i].frameCounter++;

            if (destructionQueue[i].frameCounter > (int)RHI::Limits::MaxSwapChainImageCount * 2)
            {
                delete destructionQueue[i].buffer;
                delete destructionQueue[i].srg;
                delete destructionQueue[i].material;
                destructionQueue.RemoveAt(i);
            }
        }

        if (viewConstantsUpdateRequired[imageIndex])
        {
            viewConstantsBuffer[imageIndex]->UploadData(&viewConstants, sizeof(viewConstants));

            viewConstantsUpdateRequired[imageIndex] = false;
        }

        if (quadUpdatesRequired[imageIndex])
        {
            // TODO: Implement support for more than 65,535 vertices!
            u32 totalSize = vertexArray.GetCount() * sizeof(FVertex) + indexArray.GetCount() * sizeof(FDrawIndex);

            if (totalSize > quadsBuffer[imageIndex]->GetBufferSize())
            {
                GrowQuadBuffer(totalSize);

                for (DrawPacket* drawPacket : drawPacketsPerImage[imageIndex])
                {
                    drawPacket->drawItems[0].vertexBufferViews[0] = VertexBufferView(quadsBuffer[imageIndex],
                        0,
                        vertexArray.GetCount() * sizeof(FVertex),
                        sizeof(FVertex));

                    drawPacket->drawItems[0].indexBufferView[0] = IndexBufferView(quadsBuffer[imageIndex],
                        vertexArray.GetCount() * sizeof(FVertex),
                        indexArray.GetCount() * sizeof(FDrawIndex),
                        sizeof(FDrawIndex) == 2 ? IndexFormat::Uint16 : IndexFormat::Uint32);
                }
            }

            if (totalSize > 0)
            {
                void* data;
                quadsBuffer[imageIndex]->Map(0, totalSize, &data);
                {
                    memcpy((u8*)data, 
                        vertexArray.GetData(), 
                        vertexArray.GetCount() * sizeof(FVertex));
                    memcpy((u8*)data + vertexArray.GetCount() * sizeof(FVertex),
                        indexArray.GetData(),
                        indexArray.GetCount() * sizeof(FDrawIndex));
                }
                quadsBuffer[imageIndex]->Unmap();
            }

            if (objectDataArray.GetCount() > objectDataBuffer.GetElementCount())
            {
                u32 totalElementRequired = Math::Max<u32>(objectDataArray.GetCount(), objectDataBuffer.GetElementCount() + objectDataGrowCount);
                objectDataBuffer.GrowToFit(totalElementRequired);
            }

            if (objectDataArray.GetCount() > 0)
	        {
		        void* data;
            	objectDataBuffer.Map(imageIndex, 0, objectDataArray.GetCount() * sizeof(FObjectData), &data);
	            {
		        	memcpy(data, objectDataArray.GetData(), objectDataArray.GetCount() * sizeof(FObjectData));
	            }
            	objectDataBuffer.Unmap(imageIndex);
	        }

            quadUpdatesRequired[imageIndex] = false;
        }

        return drawPacketsPerImage[imageIndex];
    }

    void FusionRenderer2::Begin()
    {
        ZoneScoped;

        Ref<FFontManager> fontManager = FusionApplication::Get()->GetFontManager();

        currentBrush = FBrush();
        currentPen = FPen();
        currentFont = FFont(fontManager->GetDefaultFontFamily(), fontManager->GetDefaultFontSize());

        coordinateSpaceStack.RemoveAll();
        drawCmdList.RemoveAll();
        vertexArray.RemoveAll();
        indexArray.RemoveAll();
        PathClear();
        objectDataArray.RemoveAll();
        indexWritePtr = nullptr;
        vertexWritePtr = nullptr;
        vertexCurrentIdx = 0;
        transformIndex = 0;

        PushChildCoordinateSpace(Matrix4x4::Identity());
    }

    void FusionRenderer2::End()
    {
        ZoneScoped;

        PopChildCoordinateSpace();

        for (int imageIdx = 0; imageIdx < numFrames; ++imageIdx)
        {
            quadUpdatesRequired[imageIdx] = true;

            Array<RHI::DrawPacket*> oldPackets = this->drawPacketsPerImage[imageIdx];

            for (int i = 0; i < drawCmdList.GetCount(); ++i)
            {
                if (drawCmdList[i].numIndices == 0)
                    continue;

                if (oldPackets.NotEmpty())
                {
                    RHI::DrawPacket* drawPacket = oldPackets[0];
                    oldPackets.RemoveAt(0);

                    RHI::DrawIndexedArguments drawArgs{};
                    drawArgs.firstIndex = drawCmdList[i].indexOffset;
                    drawArgs.firstInstance = drawCmdList[i].firstInstance;
                    drawArgs.vertexOffset = drawCmdList[i].vertexOffset;
                    drawArgs.instanceCount = 1;
                    drawArgs.indexCount = drawCmdList[i].numIndices;

                    drawPacket->drawItems[0].arguments = drawArgs;

                    drawPacket->shaderResourceGroups[0] = perObjectSrg;
                    drawPacket->shaderResourceGroups[1] = perViewSrg;
                    drawPacket->shaderResourceGroups[2] = drawCmdList[i].fontSrg;

                    drawPacket->drawItems[0].vertexBufferViews[0] = VertexBufferView(quadsBuffer[imageIdx],
                            0,
                            vertexArray.GetCount() * sizeof(FVertex),
                            sizeof(FVertex));

                    drawPacket->drawItems[0].indexBufferView[0] = IndexBufferView(quadsBuffer[imageIdx],
                            vertexArray.GetCount() * sizeof(FVertex),
                            indexArray.GetCount() * sizeof(FDrawIndex),
                            sizeof(FDrawIndex) == 2 ? IndexFormat::Uint16 : IndexFormat::Uint32);

                    drawPacket->drawItems[0].pipelineState = fusionShader->GetVariant(0)->GetPipeline(multisampling);
                }
                else
                {
                    RHI::DrawPacketBuilder builder{};

                    RHI::DrawIndexedArguments drawArgs{};
                    drawArgs.firstIndex = drawCmdList[i].indexOffset;
                    drawArgs.firstInstance = drawCmdList[i].firstInstance;
                    drawArgs.vertexOffset = drawCmdList[i].vertexOffset;
                    drawArgs.instanceCount = 1;
                    drawArgs.indexCount = drawCmdList[i].numIndices;

                    builder.SetDrawArguments(drawArgs);

                    builder.AddShaderResourceGroup(perObjectSrg);
                    builder.AddShaderResourceGroup(perViewSrg);
                    builder.AddShaderResourceGroup(drawCmdList[i].fontSrg);

                    // UI Item
                    {
                        RHI::DrawPacketBuilder::DrawItemRequest request{};
                        request.vertexBufferViews.Add(VertexBufferView(quadsBuffer[imageIdx],
                            0,
                            vertexArray.GetCount() * sizeof(FVertex),
                            sizeof(FVertex)));

                        request.indexBufferView = IndexBufferView(quadsBuffer[imageIdx],
                            vertexArray.GetCount() * sizeof(FVertex),
                            indexArray.GetCount() * sizeof(FDrawIndex),
                            sizeof(FDrawIndex) == 2 ? IndexFormat::Uint16 : IndexFormat::Uint32);

                        request.drawItemTag = drawListTag;
                        request.drawFilterMask = RHI::DrawFilterMask::ALL;

                        request.pipelineState = fusionShader->GetVariant(0)->GetPipeline(multisampling);

                        builder.AddDrawItem(request);
                    }

                    RHI::DrawPacket* drawPacket = builder.Build();
                    this->drawPacketsPerImage[imageIdx].Add(drawPacket);
                }
            }

            if (oldPackets.NotEmpty())
            {
                freePackets.AddRange(oldPackets);
                oldPackets.Clear();
            }
        }
    }

    void FusionRenderer2::PushChildCoordinateSpace(const Matrix4x4& transform)
    {
        if (coordinateSpaceStack.IsEmpty())
        {
            coordinateSpaceStack.Insert(FCoordinateSpace{ 
                .transform = transform, .translation = Vec3(), .isTranslationOnly = false });
        }
        else
        {
            if (coordinateSpaceStack.Last().isTranslationOnly)
            {
                coordinateSpaceStack.Insert(FCoordinateSpace{
	                .transform = coordinateSpaceStack.Last().transform * Matrix4x4::Translation(coordinateSpaceStack.Last().translation) * transform,
	                .translation = Vec3(),
	                .isTranslationOnly = false });
            }
            else
            {
                coordinateSpaceStack.Insert(FCoordinateSpace{
	                .transform = coordinateSpaceStack.Last().transform * transform,
	                .translation = Vec3(),
	                .isTranslationOnly = false });
            }
        }

        objectDataArray.Insert(FObjectData{ .transform = coordinateSpaceStack.Last().transform });
        transformIndex = (int)objectDataArray.GetCount() - 1;

        AddDrawCmd();
    }

    void FusionRenderer2::PushChildCoordinateSpace(Vec2 translation)
    {
        if (coordinateSpaceStack.Last().isTranslationOnly)
        {
            coordinateSpaceStack.Insert(FCoordinateSpace{
                .transform = coordinateSpaceStack.Last().transform,
                .translation = coordinateSpaceStack.Last().translation + translation,
                .isTranslationOnly = true });
        }
        else
        {
            coordinateSpaceStack.Insert(FCoordinateSpace{
                .transform = coordinateSpaceStack.Last().transform,
                .translation = translation,
                .isTranslationOnly = true });
        }
    }

    void FusionRenderer2::PopChildCoordinateSpace()
    {
        if (!coordinateSpaceStack.IsEmpty())
        {
            bool isTranslation = coordinateSpaceStack.Last().isTranslationOnly;

            coordinateSpaceStack.RemoveLast();

            if (!isTranslation && !coordinateSpaceStack.IsEmpty())
            {
                transformIndex--;
                AddDrawCmd();
            }
        }
    }

    void FusionRenderer2::SetPen(const FPen& pen)
    {
        this->currentPen = pen;
    }

    void FusionRenderer2::SetBrush(const FBrush& brush)
    {
        this->currentBrush = brush;
    }

    void FusionRenderer2::SetFont(const FFont& font)
    {
        this->currentFont = font;
    }

    void FusionRenderer2::PathClear()
    {
        path.RemoveAll();

        pathMax = Vec2(NumericLimits<f32>::Min(), NumericLimits<f32>::Min());
        pathMin = Vec2(NumericLimits<f32>::Max(), NumericLimits<f32>::Max());
    }

    void FusionRenderer2::PathLineTo(const Vec2& point)
    {
        PathInsert(point);
    }

    void FusionRenderer2::PathArcTo(const Vec2& center, float radius, float startAngle, float endAngle)
    {
        if (radius < 0.5f)
        {
            PathInsert(center);
	        return;
        }

        const float arcLength = Math::Abs(endAngle - startAngle);
        const int circleSegmentCount = CalculateNumCircleSegments(radius);
        const int arcSegmentCount = Math::Max(Math::RoundToInt(ceilf(circleSegmentCount * arcLength / (Math::PI * 2.0f))), 
            Math::RoundToInt(2.0f * Math::PI / arcLength));

        PathArcTo(center, radius, startAngle, endAngle, arcSegmentCount);
    }

    void FusionRenderer2::PathArcTo(const Vec2& center, float radius, float startAngle, float endAngle, int numSegments)
    {
        if (radius < 0.5f)
        {
            PathInsert(center);
	        return;
        }

        for (int i = 0; i <= numSegments; ++i)
        {
            const float angle = startAngle + ((float)i / (float)numSegments) * (endAngle - startAngle);
            const float degrees = Math::ToDegrees(angle);
            PathInsert(Vec2(center.x + Math::Cos(angle) * radius, center.y + Math::Sin(angle) * radius));
        }
    }

    void FusionRenderer2::PathArcToFast(const Vec2& center, float radius, int angleMinOf12, int angleMaxOf12)
    {
        if (radius < 0.5f)
        {
            PathInsert(center);
            return;
        }
        PathArcToFastInternal(center, radius, angleMinOf12 * ArcFastTableSize / 12, angleMaxOf12 * ArcFastTableSize / 12, 0);
    }

    void FusionRenderer2::PathRect(const Rect& rect, const Vec4& cornerRadius)
    {
        const Vec2& min = rect.min;
        const Vec2& max = rect.max;

        PathArcToFast(Vec2(min.x + cornerRadius.topLeft, min.y + cornerRadius.topLeft), cornerRadius.topLeft,
            6, 9);
        PathArcToFast(Vec2(max.x - cornerRadius.topRight, min.y + cornerRadius.topRight), cornerRadius.topRight,
            9, 12);
        PathArcToFast(Vec2(max.x - cornerRadius.bottomRight, max.y - cornerRadius.bottomRight), cornerRadius.bottomRight,
            0, 3);
        PathArcToFast(Vec2(min.x + cornerRadius.bottomLeft, max.y - cornerRadius.bottomLeft), cornerRadius.bottomLeft,
            3, 6);
    }

    void FusionRenderer2::PathFill(bool antiAliased)
    {
        AddConvexPolySolidFill(path.GetData(), path.GetCount(), antiAliased);

        PathClear();
    }

    void FusionRenderer2::PathStroke(bool closed, bool antiAliased)
    {
        AddPolyLine(path.GetData(), (int)path.GetCount(), currentPen.GetThickness(), closed, antiAliased);

        PathClear();
    }

    void FusionRenderer2::PathFillStroke(bool closed, bool antiAliased)
    {
        AddConvexPolySolidFill(path.GetData(), (int)path.GetCount(), antiAliased);

        AddPolyLine(path.GetData(), (int)path.GetCount(), currentPen.GetThickness(), closed, antiAliased);

        PathClear();
    }

    void FusionRenderer2::FillRect(const Rect& rect, const Vec4& cornerRadius, bool antiAliased)
    {
        AddRectFilled(rect, cornerRadius, antiAliased);
    }

    void FusionRenderer2::StrokeRect(const Rect& rect, const Vec4& cornerRadius, bool antiAliased)
    {
        AddRect(rect, cornerRadius, antiAliased);
    }

    void FusionRenderer2::FillCircle(const Vec2& center, f32 radius, bool antiAliased)
    {
        AddCircleFilled(center, radius, 0, antiAliased);
    }

    void FusionRenderer2::StrokeCircle(const Vec2& center, f32 radius, bool antiAliased)
    {
        AddCircle(center, radius, 0, antiAliased);
    }

    int FusionRenderer2::CalculateNumCircleSegments(float radius) const
    {
        const int radiusIndex = (int)(radius + 0.999999f); // ceil to never reduce accuracy
        return Math::Clamp((((((int)ceilf(Math::PI / acosf(1 - Math::Min((circleSegmentMaxError), (radius)) / (radius)))) + 1) / 2) * 2), 4, 512);
    }

    void FusionRenderer2::PathArcToFastInternal(const Vec2& center, float radius, int sampleMin, int sampleMax, int step)
    {
        if (radius < 0.5f)
        {
            PathInsert(center);
            return;
        }

        // Calculate arc auto segment step size
        if (step <= 0)
            step = ArcFastTableSize / CalculateNumCircleSegments(radius);

        // Make sure we never do steps larger than one quarter of the circle
        step = Math::Clamp(step, 1, (int)ArcFastTableSize / 4);

        const int sampleRange = abs(sampleMax - sampleMin);
        const int nextStep = step;

        int samples = sampleRange + 1;
        bool extraMaxSample = false;
        if (step > 1)
        {
            samples = sampleRange / step + 1;
            const int overstep = sampleRange % step;

            if (overstep > 0)
            {
                extraMaxSample = true;
                samples++;

                // When we have overstep to avoid awkwardly looking one long line and one tiny one at the end,
                // distribute first step range evenly between them by reducing first step size.
                if (sampleRange > 0)
                    step -= (step - overstep) / 2;
            }
        }

        path.InsertRange(samples, Vec2());

        Vec2* outPtr = path.GetData() + (path.GetCount() - samples);

        int sampleIndex = sampleMin;
        if (sampleIndex < 0 || sampleIndex >= ArcFastTableSize)
        {
            sampleIndex = sampleIndex % ArcFastTableSize;
            if (sampleIndex < 0)
                sampleIndex += ArcFastTableSize;
        }

        if (sampleMax >= sampleMin)
        {
            for (int a = sampleMin; a <= sampleMax; a += step, sampleIndex += step, step = nextStep)
            {
                if (sampleIndex >= ArcFastTableSize)
                    sampleIndex -= ArcFastTableSize;

                const Vec2 s = arcFastVertex[sampleIndex];
                outPtr->x = center.x + s.x * radius;
                outPtr->y = center.y + s.y * radius;
                PathMinMax(*outPtr);
                outPtr++;
            }
        }
        else
        {
            for (int a = sampleMin; a >= sampleMax; a -= step, sampleIndex -= step, step = nextStep)
            {
                if (sampleIndex < 0)
                    sampleIndex += ArcFastTableSize;

                const Vec2 s = arcFastVertex[sampleIndex];
                outPtr->x = center.x + s.x * radius;
                outPtr->y = center.y + s.y * radius;
                PathMinMax(*outPtr);
                outPtr++;
            }
        }

        if (extraMaxSample)
        {
            int normalizedMaxSample = sampleMax % ArcFastTableSize;
            if (normalizedMaxSample < 0)
                normalizedMaxSample += ArcFastTableSize;

            const Vec2 s = arcFastVertex[normalizedMaxSample];
            outPtr->x = center.x + s.x * radius;
            outPtr->y = center.y + s.y * radius;
            PathMinMax(*outPtr);
            outPtr++;
        }
    }

    void FusionRenderer2::AddDrawCmd()
    {
        if (coordinateSpaceStack.IsEmpty())
            return;

        // TODO: Implement vertex indexing with more than 65,535 vertices

        Ref<FFontManager> fontManager = FusionApplication::Get()->GetFontManager();

        Name fontFamily = currentFont.GetFamily();
        int fontSize = currentFont.GetFontSize();

        if (fontSize <= 0)
            fontSize = 12;
        if (!fontFamily.IsValid())
            fontFamily = fontManager->GetDefaultFontFamily();

        FFontAtlas* fontAtlas = fontManager->FindFont(fontFamily);

        FDrawCmd drawCmd{};

        if (fontAtlas)
        {
            drawCmd.fontSrg = fontAtlas->GetFontSrg2();
        }

        if (drawCmdList.GetCount() > 0)
        {
            if (drawCmdList.Last().numIndices == 0)
            {
                if (fontAtlas)
                {
                    drawCmdList.Last().fontSrg = fontAtlas->GetFontSrg2();
                }
            }
            else
            {
                drawCmd.firstInstance = transformIndex;
                drawCmd.vertexOffset = 0;
                drawCmd.indexOffset = (u32)indexArray.GetCount();
                drawCmd.numIndices = 0;
                
                drawCmdList.Insert(drawCmd);
            }
        }
        else
        {
            drawCmdList.Insert(drawCmd);
        }
    }

    void FusionRenderer2::PrimReserve(int vertexCount, int indexCount)
    {
        int curVertexCount = vertexArray.GetCount();
        vertexArray.InsertRange(vertexCount);
        vertexWritePtr = vertexArray.GetData() + curVertexCount;

        int curIndexCount = indexArray.GetCount();
        indexArray.InsertRange(indexCount);
        indexWritePtr = indexArray.GetData() + curIndexCount;
    }

    void FusionRenderer2::PrimUnreserve(int vertexCount, int indexCount)
    {
        while (vertexCount > 0)
        {
            vertexArray.RemoveLast();
            vertexCount--;
        }

        while (indexCount > 0)
        {
            indexArray.RemoveLast();
            indexCount--;
        }
    }

    void FusionRenderer2::PrimRect(const Rect& rect, u32 color)
    {
        Vec2 offset = Vec2();
        if (coordinateSpaceStack.Last().isTranslationOnly)
        {
            offset = coordinateSpaceStack.Last().translation;
        }

        Vec2 topLeft = rect.min + offset;
        Vec2 topRight = Vec2(rect.max.x, rect.min.y) + offset;
        Vec2 bottomRight = Vec2(rect.max.x, rect.max.y) + offset;
        Vec2 bottomLeft = Vec2(rect.min.x, rect.max.y) + offset;
        Vec2 uv = Vec2(0, 0);

        FDrawIndex idx = vertexCurrentIdx;
        indexWritePtr[0] = idx; indexWritePtr[1] = (idx + 1); indexWritePtr[2] = (idx + 2);
        indexWritePtr[3] = idx; indexWritePtr[4] = (idx + 2); indexWritePtr[5] = (idx + 3);

        vertexWritePtr[0].position = topLeft; vertexWritePtr[0].color = color; vertexWritePtr[0].uv = uv;
        vertexWritePtr[1].position = topRight; vertexWritePtr[1].color = color; vertexWritePtr[0].uv = uv;
        vertexWritePtr[2].position = bottomRight; vertexWritePtr[2].color = color; vertexWritePtr[0].uv = uv;
        vertexWritePtr[3].position = bottomLeft; vertexWritePtr[3].color = color; vertexWritePtr[0].uv = uv;
        vertexWritePtr += 4;
        vertexCurrentIdx += 4;
        indexWritePtr += 6;

        drawCmdList.Last().numIndices += 6;
    }

    void FusionRenderer2::PathInsert(const Vec2& point)
    {
        path.Insert(point);

        PathMinMax(point);
    }

    void FusionRenderer2::PathMinMax(const Vec2& point)
    {
        pathMin.x = Math::Min(point.x, pathMin.x);
        pathMin.y = Math::Min(point.y, pathMin.y);

        pathMax.x = Math::Max(point.x, pathMax.x);
        pathMax.y = Math::Max(point.y, pathMax.y);
    }

    void FusionRenderer2::AddRect(const Rect& rect, const Vec4& cornerRadius, bool antiAliased)
    {
        u32 color = currentPen.GetColor().ToU32();

        if ((color & ColorAlphaMask) == 0)
            return;

        PathClear();
        if (antiAliased)
            PathRect(Rect(rect.min + Vec2(0.50f, 0.50f), rect.max - Vec2(0.50f, 0.50f)), cornerRadius);
        else
            PathRect(Rect(rect.min + Vec2(0.50f, 0.50f), rect.max - Vec2(0.49f, 0.49f)), cornerRadius); // Better looking lower-right corner and rounded non-AA shapes.

    	PathStroke(true, antiAliased);
    }

    void FusionRenderer2::AddRectFilled(const Rect& rect, const Vec4& cornerRadius, bool antiAliased)
    {
        u32 color = currentBrush.GetFillColor().ToU32();

        if (cornerRadius.GetMax() < 0.5f)
        {
	        PrimReserve(4, 6);
        	PrimRect(rect, color);
        }
        else
        {
            PathClear();
            PathRect(rect, cornerRadius);
            PathFill(antiAliased);
        }
    }

    void FusionRenderer2::AddConvexPolySolidFill(const Vec2* points, int numPoints, bool antiAliased)
    {
        if (points == nullptr || numPoints <= 0)
            return;

        u32 color = currentBrush.GetFillColor().ToU32();

        Vec2 uv = whitePixelUV;

        Vec2 offset = Vec2();
        if (coordinateSpaceStack.Last().isTranslationOnly)
        {
            offset = coordinateSpaceStack.Last().translation;
        }

        if (antiAliased)
        {
            // Credit: Dear ImGui

            const float AA_SIZE = fringeScale;
            const u32 transparentColor = color & ~ColorAlphaMask;
            const int indexCount = (numPoints - 2) * 3 + numPoints * 6;
            const int vertexCount = (numPoints * 2);
            PrimReserve(vertexCount, indexCount);
            
            // Add indexes for fill
            unsigned int vertexInnerIdx = vertexCurrentIdx;
            unsigned int vertexOuterIdx = vertexCurrentIdx + 1;
            for (int i = 2; i < numPoints; i++)
            {
                indexWritePtr[0] = (FDrawIndex)(vertexInnerIdx); indexWritePtr[1] = (FDrawIndex)(vertexInnerIdx + ((i - 1) << 1)); indexWritePtr[2] = (FDrawIndex)(vertexInnerIdx + (i << 1));
                indexWritePtr += 3;
            }

            // Compute normals
            temporaryPoints.RemoveAll();
            temporaryPoints.InsertRange(numPoints);
            Vec2* tempNormals = temporaryPoints.GetData();
            
            for (int i0 = numPoints - 1, i1 = 0; i1 < numPoints; i0 = i1++)
            {
                const Vec2& p0 = points[i0] + offset;
                const Vec2& p1 = points[i1] + offset;
                float dx = p1.x - p0.x;
                float dy = p1.y - p0.y;
                IM_NORMALIZE2F_OVER_ZERO(dx, dy);
                tempNormals[i0].x = dy;
                tempNormals[i0].y = -dx;
            }

            for (int i0 = numPoints - 1, i1 = 0; i1 < numPoints; i0 = i1++)
            {
                // Average normals
                const Vec2& n0 = tempNormals[i0];
                const Vec2& n1 = tempNormals[i1];
                float dm_x = (n0.x + n1.x) * 0.5f;
                float dm_y = (n0.y + n1.y) * 0.5f;
                IM_FIXNORMAL2F(dm_x, dm_y);
                dm_x *= AA_SIZE * 0.5f;
                dm_y *= AA_SIZE * 0.5f;
                
                // Add vertices
                vertexWritePtr[0].position.x = (points[i1].x - dm_x) + offset.x; vertexWritePtr[0].position.y = (points[i1].y - dm_y) + offset.y; vertexWritePtr[0].uv = uv; vertexWritePtr[0].color = color;        // Inner
                vertexWritePtr[1].position.x = (points[i1].x + dm_x) + offset.x; vertexWritePtr[1].position.y = (points[i1].y + dm_y) + offset.y; vertexWritePtr[1].uv = uv; vertexWritePtr[1].color = transparentColor;  // Outer
                vertexWritePtr += 2;
                
                // Add indexes for fringes
                indexWritePtr[0] = (FDrawIndex)(vertexInnerIdx + (i1 << 1)); indexWritePtr[1] = (FDrawIndex)(vertexInnerIdx + (i0 << 1)); indexWritePtr[2] = (FDrawIndex)(vertexOuterIdx + (i0 << 1));
                indexWritePtr[3] = (FDrawIndex)(vertexOuterIdx + (i0 << 1)); indexWritePtr[4] = (FDrawIndex)(vertexOuterIdx + (i1 << 1)); indexWritePtr[5] = (FDrawIndex)(vertexInnerIdx + (i1 << 1));
                indexWritePtr += 6;
            }

            vertexCurrentIdx += vertexCount;
            drawCmdList.Last().numIndices += indexCount;
        }
        else
        {
            const int indexCount = (numPoints - 2) * 3;
            const int vertexCount = numPoints;
            PrimReserve(vertexCount, indexCount);

            for (int i = 0; i < vertexCount; i++)
            {
                vertexWritePtr[0].position = points[i] + offset;
                vertexWritePtr[0].uv = whitePixelUV;
                vertexWritePtr[0].color = color;
                vertexWritePtr++;
            }

            for (int i = 2; i < numPoints; ++i)
            {
                indexWritePtr[0] = vertexCurrentIdx;
                indexWritePtr[1] = vertexCurrentIdx + (i - 1);
                indexWritePtr[2] = vertexCurrentIdx + (i);
                indexWritePtr += 3;
            }

            vertexCurrentIdx += vertexCount;
            drawCmdList.Last().numIndices += indexCount;
        }
    }

    void FusionRenderer2::AddPolyLine(const Vec2* points, int numPoints, f32 thickness, bool closed, bool antiAliased)
    {
        if (points == nullptr || numPoints <= 0)
            return;

        u32 color = currentPen.GetColor().ToU32();

        const Vec2 uv = whitePixelUV;
        const int count = closed ? numPoints : numPoints - 1; // The number of line segments we need to draw
        const bool thickLine = (thickness > fringeScale);

        Vec2 offset = Vec2();
        if (coordinateSpaceStack.Last().isTranslationOnly)
        {
            offset = coordinateSpaceStack.Last().translation;
        }

        // Credit: Dear ImGui

        if (antiAliased)
        {
	        // [PATH 1]
            // Anti-aliased stroke
            const float AA_SIZE = fringeScale;
            const u32 transparentColor = color & ~ColorAlphaMask;

            // Thicknesses <1.0 should behave like thickness 1.0
            thickness = Math::Max(thickness, 1.0f);
            const int integerThickness = (int)thickness;
            const float fractionalThickness = thickness - integerThickness;

            const int indexCount = (thickLine ? count * 18 : count * 12);
            const int vertexCount = (thickLine ? numPoints * 4 : numPoints * 3);
            PrimReserve(vertexCount, indexCount);

            // Temporary buffer
			// The first <numPoints> items are normals at each line point, then after that there are either 2 or 4 temp points for each line point
            temporaryPoints.RemoveAll();
            temporaryPoints.InsertRange(numPoints * (thickLine ? 5 : 3));
            Vec2* tempNormals = temporaryPoints.GetData();
            Vec2* tempPoints = tempNormals + numPoints;

            // Calculate normals (tangents) for each line segment
            for (int i1 = 0; i1 < count; i1++)
            {
                const int i2 = (i1 + 1) == numPoints ? 0 : i1 + 1;
                float dx = points[i2].x - points[i1].x;
                float dy = points[i2].y - points[i1].y;
                IM_NORMALIZE2F_OVER_ZERO(dx, dy);
                tempNormals[i1].x = dy;
                tempNormals[i1].y = -dx;
            }

            if (!closed)
                tempNormals[numPoints - 1] = tempNormals[numPoints - 2];

            if (!thickLine)
            {
                const float half_draw_size = AA_SIZE;

                // If line is not closed, the first and last points need to be generated differently as there are no normals to blend
                if (!closed)
                {
                    tempPoints[0] = points[0] + tempNormals[0] * half_draw_size + offset;
                    tempPoints[1] = points[0] - tempNormals[0] * half_draw_size + offset;
                    tempPoints[(numPoints - 1) * 2 + 0] = points[numPoints - 1] + tempNormals[numPoints - 1] * half_draw_size + offset;
                    tempPoints[(numPoints - 1) * 2 + 1] = points[numPoints - 1] - tempNormals[numPoints - 1] * half_draw_size + offset;
                }

                // Generate the indices to form a number of triangles for each line segment, and the vertices for the line edges
                // This takes points n and n+1 and writes into n+1, with the first point in a closed line being generated from the final one (as n+1 wraps)
                // FIXME-OPT: Merge the different loops, possibly remove the temporary buffer.
                unsigned int idx1 = vertexCurrentIdx; // Vertex index for start of line segment
                for (int i1 = 0; i1 < count; i1++) // i1 is the first point of the line segment
                {
                    const int i2 = (i1 + 1) == numPoints ? 0 : i1 + 1; // i2 is the second point of the line segment
                    const unsigned int idx2 = ((i1 + 1) == numPoints) ? vertexCurrentIdx : (idx1 + 3); // Vertex index for end of segment

                    // Average normals
                    float dm_x = (tempNormals[i1].x + tempNormals[i2].x) * 0.5f;
                    float dm_y = (tempNormals[i1].y + tempNormals[i2].y) * 0.5f;
                    IM_FIXNORMAL2F(dm_x, dm_y);
                    dm_x *= half_draw_size; // dm_x, dm_y are offset to the outer edge of the AA area
                    dm_y *= half_draw_size;

                    // Add temporary vertexes for the outer edges
                    Vec2* out_vtx = &tempPoints[i2 * 2];
                    out_vtx[0].x = points[i2].x + dm_x + offset.x;
                    out_vtx[0].y = points[i2].y + dm_y + offset.y;
                    out_vtx[1].x = points[i2].x - dm_x + offset.x;
                    out_vtx[1].y = points[i2].y - dm_y + offset.y;

                    {
                        // Add indexes for four triangles
                        indexWritePtr[0] = (FDrawIndex)(idx2 + 0); indexWritePtr[1] = (FDrawIndex)(idx1 + 0); indexWritePtr[2] = (FDrawIndex)(idx1 + 2); // Right tri 1
                        indexWritePtr[3] = (FDrawIndex)(idx1 + 2); indexWritePtr[4] = (FDrawIndex)(idx2 + 2); indexWritePtr[5] = (FDrawIndex)(idx2 + 0); // Right tri 2
                        indexWritePtr[6] = (FDrawIndex)(idx2 + 1); indexWritePtr[7] = (FDrawIndex)(idx1 + 1); indexWritePtr[8] = (FDrawIndex)(idx1 + 0); // Left tri 1
                        indexWritePtr[9] = (FDrawIndex)(idx1 + 0); indexWritePtr[10] = (FDrawIndex)(idx2 + 0); indexWritePtr[11] = (FDrawIndex)(idx2 + 1); // Left tri 2
                        indexWritePtr += 12;
                    }

                    idx1 = idx2;
                }

                {
                    // If we're not using a texture, we need the center vertex as well
                    for (int i = 0; i < numPoints; i++)
                    {
                        vertexWritePtr[0].position = points[i] + offset;              vertexWritePtr[0].uv = whitePixelUV; vertexWritePtr[0].color = color;       // Center of line
                        vertexWritePtr[1].position = tempPoints[i * 2 + 0]; vertexWritePtr[1].uv = whitePixelUV; vertexWritePtr[1].color = transparentColor; // Left-side outer edge
                        vertexWritePtr[2].position = tempPoints[i * 2 + 1]; vertexWritePtr[2].uv = whitePixelUV; vertexWritePtr[2].color = transparentColor; // Right-side outer edge
                        vertexWritePtr += 3;
                    }
                }
            }
            else
            {
                // [PATH 2] Non texture-based lines (thick): we need to draw the solid line core and thus require four vertices per point
                const float half_inner_thickness = (thickness - AA_SIZE) * 0.5f;

                // If line is not closed, the first and last points need to be generated differently as there are no normals to blend
                if (!closed)
                {
                    const int points_last = numPoints - 1;
                    tempPoints[0] = points[0] + offset + tempNormals[0] * (half_inner_thickness + AA_SIZE);
                    tempPoints[1] = points[0] + offset + tempNormals[0] * (half_inner_thickness);
                    tempPoints[2] = points[0] + offset - tempNormals[0] * (half_inner_thickness);
                    tempPoints[3] = points[0] + offset - tempNormals[0] * (half_inner_thickness + AA_SIZE);
                    tempPoints[points_last * 4 + 0] = points[points_last] + offset + tempNormals[points_last] * (half_inner_thickness + AA_SIZE);
                    tempPoints[points_last * 4 + 1] = points[points_last] + offset + tempNormals[points_last] * (half_inner_thickness);
                    tempPoints[points_last * 4 + 2] = points[points_last] + offset - tempNormals[points_last] * (half_inner_thickness);
                    tempPoints[points_last * 4 + 3] = points[points_last] + offset - tempNormals[points_last] * (half_inner_thickness + AA_SIZE);
                }

                // Generate the indices to form a number of triangles for each line segment, and the vertices for the line edges
                // This takes points n and n+1 and writes into n+1, with the first point in a closed line being generated from the final one (as n+1 wraps)
                // FIXME-OPT: Merge the different loops, possibly remove the temporary buffer.
                unsigned int idx1 = vertexCurrentIdx; // Vertex index for start of line segment
                for (int i1 = 0; i1 < count; i1++) // i1 is the first point of the line segment
                {
                    const int i2 = (i1 + 1) == numPoints ? 0 : (i1 + 1); // i2 is the second point of the line segment
                    const unsigned int idx2 = (i1 + 1) == numPoints ? vertexCurrentIdx : (idx1 + 4); // Vertex index for end of segment

                    // Average normals
                    float dm_x = (tempNormals[i1].x + tempNormals[i2].x) * 0.5f;
                    float dm_y = (tempNormals[i1].y + tempNormals[i2].y) * 0.5f;
                    IM_FIXNORMAL2F(dm_x, dm_y);
                    float dm_out_x = dm_x * (half_inner_thickness + AA_SIZE);
                    float dm_out_y = dm_y * (half_inner_thickness + AA_SIZE);
                    float dm_in_x = dm_x * half_inner_thickness;
                    float dm_in_y = dm_y * half_inner_thickness;

                    // Add temporary vertices
                    Vec2* out_vtx = &tempPoints[i2 * 4];
                    out_vtx[0].x = points[i2].x + dm_out_x + offset.x;
                    out_vtx[0].y = points[i2].y + dm_out_y + offset.y;
                    out_vtx[1].x = points[i2].x + dm_in_x + offset.x;
                    out_vtx[1].y = points[i2].y + dm_in_y + offset.y;
                    out_vtx[2].x = points[i2].x - dm_in_x + offset.x;
                    out_vtx[2].y = points[i2].y - dm_in_y + offset.y;
                    out_vtx[3].x = points[i2].x - dm_out_x + offset.x;
                    out_vtx[3].y = points[i2].y - dm_out_y + offset.y;

                    // Add indexes
                    indexWritePtr[0] = (FDrawIndex)(idx2 + 1); indexWritePtr[1] = (FDrawIndex)(idx1 + 1); indexWritePtr[2] = (FDrawIndex)(idx1 + 2);
                    indexWritePtr[3] = (FDrawIndex)(idx1 + 2); indexWritePtr[4] = (FDrawIndex)(idx2 + 2); indexWritePtr[5] = (FDrawIndex)(idx2 + 1);
                    indexWritePtr[6] = (FDrawIndex)(idx2 + 1); indexWritePtr[7] = (FDrawIndex)(idx1 + 1); indexWritePtr[8] = (FDrawIndex)(idx1 + 0);
                    indexWritePtr[9] = (FDrawIndex)(idx1 + 0); indexWritePtr[10] = (FDrawIndex)(idx2 + 0); indexWritePtr[11] = (FDrawIndex)(idx2 + 1);
                    indexWritePtr[12] = (FDrawIndex)(idx2 + 2); indexWritePtr[13] = (FDrawIndex)(idx1 + 2); indexWritePtr[14] = (FDrawIndex)(idx1 + 3);
                    indexWritePtr[15] = (FDrawIndex)(idx1 + 3); indexWritePtr[16] = (FDrawIndex)(idx2 + 3); indexWritePtr[17] = (FDrawIndex)(idx2 + 2);
                    indexWritePtr += 18;

                    idx1 = idx2;
                }

                // Add vertices
                for (int i = 0; i < numPoints; i++)
                {
                    vertexWritePtr[0].position = tempPoints[i * 4 + 0]; vertexWritePtr[0].uv = whitePixelUV; vertexWritePtr[0].color = transparentColor;
                    vertexWritePtr[1].position = tempPoints[i * 4 + 1]; vertexWritePtr[1].uv = whitePixelUV; vertexWritePtr[1].color = color;
                    vertexWritePtr[2].position = tempPoints[i * 4 + 2]; vertexWritePtr[2].uv = whitePixelUV; vertexWritePtr[2].color = color;
                    vertexWritePtr[3].position = tempPoints[i * 4 + 3]; vertexWritePtr[3].uv = whitePixelUV; vertexWritePtr[3].color = transparentColor;
                    vertexWritePtr += 4;
                }
            }

            vertexCurrentIdx += vertexCount;
            drawCmdList.Last().numIndices += indexCount;
        }
        else
        {
            // [PATH 4] Non texture-based, Non anti-aliased lines
            const int indexCount = count * 6;
            const int vertexCount = count * 4;    // FIXME-OPT: Not sharing edges
            PrimReserve(vertexCount, indexCount);

            for (int i1 = 0; i1 < count; i1++)
            {
                const int i2 = (i1 + 1) == numPoints ? 0 : i1 + 1;
                const Vec2& p1 = points[i1] + offset;
                const Vec2& p2 = points[i2] + offset;

                float dx = p2.x - p1.x;
                float dy = p2.y - p1.y;
                IM_NORMALIZE2F_OVER_ZERO(dx, dy);
                dx *= (thickness * 0.5f);
                dy *= (thickness * 0.5f);

                vertexWritePtr[0].position.x = p1.x + dy; vertexWritePtr[0].position.y = p1.y - dx; vertexWritePtr[0].uv = whitePixelUV; vertexWritePtr[0].color = color;
                vertexWritePtr[1].position.x = p2.x + dy; vertexWritePtr[1].position.y = p2.y - dx; vertexWritePtr[1].uv = whitePixelUV; vertexWritePtr[1].color = color;
                vertexWritePtr[2].position.x = p2.x - dy; vertexWritePtr[2].position.y = p2.y + dx; vertexWritePtr[2].uv = whitePixelUV; vertexWritePtr[2].color = color;
                vertexWritePtr[3].position.x = p1.x - dy; vertexWritePtr[3].position.y = p1.y + dx; vertexWritePtr[3].uv = whitePixelUV; vertexWritePtr[3].color = color;
                vertexWritePtr += 4;

                indexWritePtr[0] = (FDrawIndex)(vertexCurrentIdx); indexWritePtr[1] = (FDrawIndex)(vertexCurrentIdx + 1); indexWritePtr[2] = (FDrawIndex)(vertexCurrentIdx + 2);
                indexWritePtr[3] = (FDrawIndex)(vertexCurrentIdx); indexWritePtr[4] = (FDrawIndex)(vertexCurrentIdx + 2); indexWritePtr[5] = (FDrawIndex)(vertexCurrentIdx + 3);
                indexWritePtr += 6;
                vertexCurrentIdx += 4;
            }

            vertexCurrentIdx += vertexCount;
            drawCmdList.Last().numIndices += indexCount;
        }
    }

    void FusionRenderer2::AddCircle(const Vec2& center, f32 radius, int numSegments, bool antiAliased)
    {
        u32 color = currentPen.GetColor().ToU32();
        if ((color & ColorAlphaMask) == 0 || radius < 0.5f)
            return;

        if (numSegments <= 0)
        {
            PathArcToFast(center, radius - 0.5f, 0, 12);
            path.RemoveLast();
        }
        else
        {
            // Explicit segment count (still clamp to avoid drawing insanely tessellated shapes)
            numSegments = Math::Clamp(numSegments, 3, CircleAutoSegmentMax);

            // Because we are filling a closed shape we remove 1 from the count of segments/points
            const float angleMax = (Math::PI * 2.0f) * ((float)numSegments - 1.0f) / (float)numSegments;
            PathArcTo(center, radius - 0.5f, 0, angleMax, numSegments - 1);
        }

        PathStroke(true, antiAliased);
    }

    void FusionRenderer2::AddCircleFilled(const Vec2& center, f32 radius, int numSegments, bool antiAliased)
    {
        u32 color = currentBrush.GetFillColor().ToU32();
        if ((color & ColorAlphaMask) == 0 || radius < 0.5f)
            return;

        if (numSegments <= 0)
        {
            PathArcToFast(center, radius, 0, 12);
            path.RemoveLast();
        }
        else
        {
            // Explicit segment count (still clamp to avoid drawing insanely tessellated shapes)
            numSegments = Math::Clamp(numSegments, 3, CircleAutoSegmentMax);

            // Because we are filling a closed shape we remove 1 from the count of segments/points
            const float angleMax = (Math::PI * 2.0f) * ((float)numSegments - 1.0f) / (float)numSegments;
            PathArcTo(center, radius, 0, angleMax, numSegments - 1);
        }

        PathFill(antiAliased);
    }

    void FusionRenderer2::GrowQuadBuffer(u64 newTotalSize)
    {
	    meshBufferGrowRatio = Math::Min<float>(meshBufferGrowRatio, 0.75f);

        u64 curSize = quadsBuffer[0]->GetBufferSize();
        newTotalSize = Math::Max(curSize, newTotalSize);

        newTotalSize = Math::Max(newTotalSize, (u64)(curSize * (1 + meshBufferGrowRatio)));

	    for (int i = 0; i < numFrames; ++i)
	    {
            RHI::Buffer* original = quadsBuffer[i];

            RHI::BufferDescriptor quadMeshBufferDesc{};
            quadMeshBufferDesc.name = "Quad Meshes";
            quadMeshBufferDesc.bindFlags = RHI::BufferBindFlags::IndexBuffer | RHI::BufferBindFlags::VertexBuffer;
            quadMeshBufferDesc.bufferSize = newTotalSize;
            quadMeshBufferDesc.defaultHeapType = MemoryHeapType::Upload;
            quadMeshBufferDesc.structureByteStride = quadMeshBufferDesc.bufferSize;

            quadsBuffer[i] = RHI::gDynamicRHI->CreateBuffer(quadMeshBufferDesc);
            quadUpdatesRequired[i] = true;

            void* src;
            original->Map(0, original->GetBufferSize(), &src);
            {
                quadsBuffer[i]->UploadData(src, original->GetBufferSize());
            }
            original->Unmap();

            QueueDestroy(original);
	    }

    }

    void FusionRenderer2::QueueDestroy(RHI::Buffer* buffer)
    {
        destructionQueue.Add({ .buffer = buffer });
    }

} // namespace CE


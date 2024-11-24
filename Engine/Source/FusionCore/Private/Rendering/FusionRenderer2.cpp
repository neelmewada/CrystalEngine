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
            quadMeshBufferDesc.bufferSize = initialQuadBufferSize;
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
        itemTransform = Matrix4x4::Identity();

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

    void FusionRenderer2::PathRect(const Rect& rect, const Vec4& cornerRadius)
    {
        const Vec2& min = rect.min;
        const Vec2& max = rect.max;

        PathArcTo(Vec2(min.x + cornerRadius.topLeft, min.y + cornerRadius.topLeft), cornerRadius.topLeft,
            Math::ToRadians(180), Math::ToRadians(270));
        PathArcTo(Vec2(max.x - cornerRadius.topRight, min.y + cornerRadius.topRight), cornerRadius.topRight,
            Math::ToRadians(270), Math::ToRadians(360));
        PathArcTo(Vec2(max.x - cornerRadius.bottomRight, max.y - cornerRadius.bottomRight), cornerRadius.bottomRight,
            Math::ToRadians(0), Math::ToRadians(90));
        PathArcTo(Vec2(min.x + cornerRadius.bottomLeft, max.y - cornerRadius.bottomLeft), cornerRadius.bottomLeft,
            Math::ToRadians(90), Math::ToRadians(180));

    }

    void FusionRenderer2::PathFill(bool antiAliased)
    {
	    switch (currentBrush.GetBrushStyle())
	    {
	    case FBrushStyle::None:
		    break;
	    case FBrushStyle::SolidFill:
		    {
			    Color fillColor = currentBrush.GetFillColor();
                if (fillColor.a > MinOpacity)
                {
                    AddConvexPolySolidFill(path.GetData(), (int)path.GetCount(), fillColor.ToU32(), antiAliased);
                }
		    }
		    break;
	    case FBrushStyle::Image:
		    break;
	    case FBrushStyle::LinearGradient:
		    break;
	    }

        PathClear();
    }

    void FusionRenderer2::DrawRect(const Rect& rect)
    {
	    switch (currentBrush.GetBrushStyle())
	    {
	    case FBrushStyle::SolidFill:
            AddRectFilled(rect, currentBrush.GetFillColor().ToU32());
            break;
	    case FBrushStyle::LinearGradient:
            break;
        default:
            break;
	    }

        
    }

    int FusionRenderer2::CalculateNumCircleSegments(float radius) const
    {
        const int radiusIndex = (int)(radius + 0.999999f); // ceil to never reduce accuracy
        return Math::Clamp((((((int)ceilf(Math::PI / acosf(1 - Math::Min((circleSegmentMaxError), (radius)) / (radius)))) + 1) / 2) * 2), 4, 512);
    }

    void FusionRenderer2::AddDrawCmd()
    {
        if (coordinateSpaceStack.IsEmpty())
            return;

        // TODO: Implement vertex indexing with more than 65,535 vertices

        FDrawCmd drawCmd{};
        if (drawCmdList.GetCount() > 0)
        {
            if (drawCmdList.Last().numIndices == 0)
            {
	            // Do nothing
            }
            else
            {
                drawCmd.firstInstance = transformIndex;//drawCmdList.Last().firstInstance + 1;
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

        pathMin.x = Math::Min(point.x, pathMin.x);
        pathMin.y = Math::Min(point.y, pathMin.y);

        pathMax.x = Math::Max(point.x, pathMax.x);
        pathMax.y = Math::Max(point.y, pathMax.y);
    }

    void FusionRenderer2::AddRectFilled(const Rect& rect, u32 color, const Vec4& cornerRadius)
    {
        if (cornerRadius.GetMax() < 0.5f)
        {
	        PrimReserve(4, 6);
        	PrimRect(rect, color);
        }
        else
        {
	        
        }
    }

    void FusionRenderer2::AddConvexPolySolidFill(const Vec2* points, int numPoints, u32 color, bool antiAliased)
    {
        if (points == nullptr || numPoints <= 0)
            return;

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

    void FusionRenderer2::GrowQuadBuffer(u64 newTotalSize)
    {
	    quadBufferGrowRatio = Math::Min<float>(quadBufferGrowRatio, 0.75f);

        u64 curSize = quadsBuffer[0]->GetBufferSize();
        newTotalSize = Math::Max(curSize, newTotalSize);

        newTotalSize = Math::Max(newTotalSize, (u64)(curSize * (1 + quadBufferGrowRatio)));

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


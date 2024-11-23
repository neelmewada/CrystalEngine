#include <algorithm>

#include "FusionCore.h"

namespace CE
{

    FusionRenderer2::FusionRenderer2()
    {

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
        }

        perViewSrg->FlushBindings();
        perObjectSrg->FlushBindings();
    }

    void FusionRenderer2::OnBeginDestroy()
    {
	    Super::OnBeginDestroy();

        objectDataBuffer.Shutdown();

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
            u32 totalSize = vertexArray.GetCount() * sizeof(FVertex) + indexArray.GetCount() * sizeof(FIndex);

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
                        indexArray.GetCount() * sizeof(FIndex));
                }
                quadsBuffer[imageIndex]->Unmap();
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
        pathPoints.RemoveAll();
        indexWritePtr = nullptr;
        vertexWritePtr = nullptr;
        vertexCurrentIdx = 0;

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
                if (oldPackets.NotEmpty())
                {
                    RHI::DrawPacket* drawPacket = oldPackets[i];
                    oldPackets.RemoveAt(i);

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
                            indexArray.GetCount() * sizeof(FIndex),
                            sizeof(FIndex) == 2 ? IndexFormat::Uint16 : IndexFormat::Uint32);

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
                            indexArray.GetCount() * sizeof(FIndex),
                            sizeof(FIndex) == 2 ? IndexFormat::Uint16 : IndexFormat::Uint32);

                        request.drawItemTag = drawListTag;
                        request.drawFilterMask = RHI::DrawFilterMask::ALL;

                        request.pipelineState = fusionShader->GetVariant(0)->GetPipeline(multisampling);

                        builder.AddDrawItem(request);
                    }

                    RHI::DrawPacket* drawPacket = builder.Build();
                    this->drawPacketsPerImage[imageIdx].Add(drawPacket);
                }
            }
        }
    }

    void FusionRenderer2::PushChildCoordinateSpace(const Matrix4x4& transform)
    {
        if (coordinateSpaceStack.IsEmpty())
        {
            coordinateSpaceStack.Insert(transform);
        }
        else
        {
            coordinateSpaceStack.Insert(coordinateSpaceStack.Last() * transform);
        }

        AddDrawCmd();
    }

    void FusionRenderer2::PopChildCoordinateSpace()
    {
        if (!coordinateSpaceStack.IsEmpty())
        {
            coordinateSpaceStack.RemoveLast();

            AddDrawCmd();
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

    void FusionRenderer2::PrimReserve(int vertexCount, int indexCount)
    {
        int curVertexCount = vertexArray.GetCount();
        vertexArray.InsertRange(curVertexCount + vertexCount);
        vertexWritePtr = vertexArray.GetData() + curVertexCount;

        int curIndexCount = indexArray.GetCount();
        indexArray.InsertRange(curIndexCount + indexCount);
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
        Vec2 topLeft = rect.min;
        Vec2 topRight = Vec2(rect.max.x, rect.min.y);
        Vec2 bottomRight = Vec2(rect.max.x, rect.max.y);
        Vec2 bottomLeft = Vec2(rect.min.x, rect.max.y);
        Vec2 uv = Vec2(0, 0);

        FIndex idx = vertexCurrentIdx;
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

    void FusionRenderer2::AddDrawCmd()
    {
        FDrawCmd drawCmd{};
        if (drawCmdList.GetCount() > 0)
        {
            drawCmd.firstInstance = drawCmdList.Last().firstInstance + 1;
            drawCmd.vertexOffset = (u32)vertexArray.GetCount();
            drawCmd.indexOffset = (u32)indexArray.GetCount();
            drawCmd.numIndices = 0;
        }
        drawCmdList.Insert(drawCmd);
    }

    void FusionRenderer2::AddRectFilled(const Rect& rect, u32 color)
    {
        PrimReserve(4, 6);
        PrimRect(rect, color);

        drawCmdList.Last().numIndices += 6;
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


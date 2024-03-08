#include "CoreRPI.h"

namespace CE::RPI
{

    ModelLodAsset::ModelLodAsset()
    {

    }

    ModelLodAsset::~ModelLodAsset()
    {

    }

    ModelLod* ModelLodAsset::CreateModelLod()
    {
        ModelLod* model = new ModelLod();

        VertexBufferList vertexBufferInfos{};
        
        u64 totalBufferSize = positionsData.GetDataSize() + normalData.GetDataSize() + tangentData.GetDataSize() + color0Data.GetDataSize() +
            uv0Data.GetDataSize() + uv1Data.GetDataSize() + uv2Data.GetDataSize() + uv3Data.GetDataSize();
        
        u64 totalIndexBufferDataSize = 0;

        for (const auto& subMeshData : subMeshes)
        {
            totalIndexBufferDataSize += subMeshData.indicesData.GetDataSize();
        }

        totalBufferSize += totalIndexBufferDataSize;

        RHI::BufferDescriptor bufferDesc{};
        bufferDesc.bindFlags = RHI::BufferBindFlags::VertexBuffer | RHI::BufferBindFlags::IndexBuffer;
        bufferDesc.bufferSize = totalBufferSize;
        bufferDesc.defaultHeapType = RHI::MemoryHeapType::Default;
        bufferDesc.name = "MeshLod Buffer";

        RHI::Buffer* buffer = RHI::gDynamicRHI->CreateBuffer(bufferDesc);
        
        model->TrackBuffer(buffer);

        auto queue = RHI::gDynamicRHI->GetPrimaryGraphicsQueue();
        RHI::Fence* fence = nullptr;
        RHI::CommandList* cmdList = nullptr;
        RHI::Buffer* stagingBuffer = nullptr;
        void* data = nullptr;

        // Map
        if (buffer->IsHostAccessible())
        {
            buffer->Map(0, buffer->GetBufferSize(), &data);
        }
        else
        {
            cmdList = RHI::gDynamicRHI->AllocateCommandList(queue);
            fence = RHI::gDynamicRHI->CreateFence(false);

            bufferDesc.bindFlags = RHI::BufferBindFlags::StagingBuffer;
            bufferDesc.defaultHeapType = MemoryHeapType::Upload;

            stagingBuffer = RHI::gDynamicRHI->CreateBuffer(bufferDesc);

            stagingBuffer->Map(0, stagingBuffer->GetBufferSize(), &data);
        }

        // Copy data
        u8* dataPtr = (u8*)data;
        u64 offset = 0;

        {
            VertexBufferInfo vertInfo{};
            vertInfo.attributeType = RHI::VertexAttributeDataType::Float3;
            vertInfo.bufferIndex = 0;
            vertInfo.byteOffset = offset;
            vertInfo.byteCount = positionsData.GetDataSize();
            vertInfo.stride = sizeof(Vec3);

            vertInfo.semantic = RHI::ShaderSemantic(RHI::VertexInputAttribute::Position);
            vertexBufferInfos.Add(vertInfo);

            memcpy(dataPtr + vertInfo.byteOffset, positionsData.GetDataPtr(), vertInfo.byteCount);
            offset += vertInfo.byteCount;
        }

        if (normalData.IsValid())
        {
            VertexBufferInfo vertInfo{};
            vertInfo.attributeType = RHI::VertexAttributeDataType::Float3;
            vertInfo.bufferIndex = 0;
            vertInfo.byteOffset = offset;
            vertInfo.byteCount = normalData.GetDataSize();
            vertInfo.stride = sizeof(Vec3);

            vertInfo.semantic = RHI::ShaderSemantic(RHI::VertexInputAttribute::Normal);
            vertexBufferInfos.Add(vertInfo);

            memcpy(dataPtr + vertInfo.byteOffset, normalData.GetDataPtr(), vertInfo.byteCount);
            offset += vertInfo.byteCount;
        }

        if (tangentData.IsValid())
        {
            VertexBufferInfo vertInfo{};
            vertInfo.attributeType = RHI::VertexAttributeDataType::Float3;
            vertInfo.bufferIndex = 0;
            vertInfo.byteOffset = offset;
            vertInfo.byteCount = tangentData.GetDataSize();
            vertInfo.stride = sizeof(Vec3);

            vertInfo.semantic = RHI::ShaderSemantic(RHI::VertexInputAttribute::Tangent);
            vertexBufferInfos.Add(vertInfo);

            memcpy(dataPtr + vertInfo.byteOffset, tangentData.GetDataPtr(), vertInfo.byteCount);
            offset += vertInfo.byteCount;
        }

        if (color0Data.IsValid())
        {
            VertexBufferInfo vertInfo{};
            vertInfo.attributeType = RHI::VertexAttributeDataType::Float4;
            vertInfo.bufferIndex = 0;
            vertInfo.byteOffset = offset;
            vertInfo.byteCount = color0Data.GetDataSize();
            vertInfo.stride = sizeof(Vec4);

            vertInfo.semantic = RHI::ShaderSemantic(RHI::VertexInputAttribute::Color, 0);
            vertexBufferInfos.Add(vertInfo);

            memcpy(dataPtr + vertInfo.byteOffset, color0Data.GetDataPtr(), vertInfo.byteCount);
            offset += vertInfo.byteCount;
        }

        if (uv0Data.IsValid())
        {
            VertexBufferInfo vertInfo{};
            vertInfo.attributeType = RHI::VertexAttributeDataType::Float2;
            vertInfo.bufferIndex = 0;
            vertInfo.byteOffset = offset;
            vertInfo.byteCount = uv0Data.GetDataSize();
            vertInfo.stride = sizeof(Vec2);

            vertInfo.semantic = RHI::ShaderSemantic(RHI::VertexInputAttribute::UV, 0);
            vertexBufferInfos.Add(vertInfo);

            memcpy(dataPtr + vertInfo.byteOffset, uv0Data.GetDataPtr(), vertInfo.byteCount);
            offset += vertInfo.byteCount;
        }

        if (uv1Data.IsValid())
        {
            VertexBufferInfo vertInfo{};
            vertInfo.attributeType = RHI::VertexAttributeDataType::Float2;
            vertInfo.bufferIndex = 0;
            vertInfo.byteOffset = offset;
            vertInfo.byteCount = uv1Data.GetDataSize();
            vertInfo.stride = sizeof(Vec2);

            vertInfo.semantic = RHI::ShaderSemantic(RHI::VertexInputAttribute::UV, 1);
            vertexBufferInfos.Add(vertInfo);

            memcpy(dataPtr + vertInfo.byteOffset, uv1Data.GetDataPtr(), vertInfo.byteCount);
            offset += vertInfo.byteCount;
        }

        if (uv2Data.IsValid())
        {
            VertexBufferInfo vertInfo{};
            vertInfo.attributeType = RHI::VertexAttributeDataType::Float2;
            vertInfo.bufferIndex = 0;
            vertInfo.byteOffset = offset;
            vertInfo.byteCount = uv2Data.GetDataSize();
            vertInfo.stride = sizeof(Vec2);

            vertInfo.semantic = RHI::ShaderSemantic(RHI::VertexInputAttribute::UV, 2);
            vertexBufferInfos.Add(vertInfo);

            memcpy(dataPtr + vertInfo.byteOffset, uv2Data.GetDataPtr(), vertInfo.byteCount);
            offset += vertInfo.byteCount;
        }

        if (uv3Data.IsValid())
        {
            VertexBufferInfo vertInfo{};
            vertInfo.attributeType = RHI::VertexAttributeDataType::Float2;
            vertInfo.bufferIndex = 0;
            vertInfo.byteOffset = offset;
            vertInfo.byteCount = uv3Data.GetDataSize();
            vertInfo.stride = sizeof(Vec2);

            vertInfo.semantic = RHI::ShaderSemantic(RHI::VertexInputAttribute::UV, 3);
            vertexBufferInfos.Add(vertInfo);

            memcpy(dataPtr + vertInfo.byteOffset, uv3Data.GetDataPtr(), vertInfo.byteCount);
            offset += vertInfo.byteCount;
        }

        u64 indexBufferOffset = 0;

        for (const auto& subMeshData : subMeshes)
        {
            RPI::Mesh subMesh{};

            RHI::DrawIndexedArguments drawArgs{};
            drawArgs.firstIndex = 0;
            drawArgs.indexCount = subMeshData.numIndices;
            drawArgs.firstInstance = 0;
            drawArgs.instanceCount = 1;
            drawArgs.vertexOffset = 0;
            subMesh.drawArguments = RHI::DrawArguments(drawArgs);

            subMesh.vertexBufferInfos = vertexBufferInfos;

            subMesh.materialSlotId = subMeshData.materialIndex;
            subMesh.indexBufferView = RHI::IndexBufferView(buffer, totalBufferSize - totalIndexBufferDataSize + indexBufferOffset, 
                subMeshData.indicesData.GetDataSize(), subMeshData.indexFormat);

            memcpy(dataPtr + subMesh.indexBufferView.GetByteOffset(), subMeshData.indicesData.GetDataPtr(), subMesh.indexBufferView.GetByteCount());

            indexBufferOffset += subMesh.indexBufferView.GetByteCount();

            model->AddMesh(subMesh);
        }
        
        // Unmap
        if (buffer->IsHostAccessible())
        {
            buffer->Unmap();
        }
        else
        {
            stagingBuffer->Unmap();

            cmdList->Begin();

            RHI::BufferCopy copy{};
            copy.srcBuffer = stagingBuffer;
            copy.dstBuffer = buffer;
            copy.srcOffset = 0;
            copy.dstOffset = 0;
            copy.totalByteSize = buffer->GetBufferSize();

            cmdList->CopyBufferRegion(copy);

            cmdList->End();

            queue->Execute(1, &cmdList, fence);
            fence->WaitForFence();

            delete cmdList;
            delete fence;
        }

        return model;
    }

} // namespace CE::RPI

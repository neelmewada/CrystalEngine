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

            delete stagingBuffer;
            delete cmdList;
            delete fence;
        }

        return model;
    }

    ModelLodAsset* ModelLodAsset::CreateCubeAsset(Object* outer)
    {
        static const Vec3 vertices[] = {
            Vec3(0.5, -0.5, 0.5),
            Vec3(-0.5, -0.5, 0.5),
            Vec3(0.5, 0.5, 0.5),
            Vec3(-0.5, 0.5, 0.5),

            Vec3(0.5, 0.5, -0.5),
            Vec3(-0.5, 0.5, -0.5),
            Vec3(0.5, -0.5, -0.5),
            Vec3(-0.5, -0.5, -0.5),

            Vec3(0.5, 0.5, 0.5),
            Vec3(-0.5, 0.5, 0.5),
            Vec3(0.5, 0.5, -0.5),
            Vec3(-0.5, 0.5, -0.5),

            Vec3(0.5, -0.5, -0.5),
            Vec3(0.5, -0.5, 0.5),
            Vec3(-0.5, -0.5, 0.5),
            Vec3(-0.5, -0.5, -0.5),

            Vec3(-0.5, -0.5, 0.5),
            Vec3(-0.5, 0.5, 0.5),
            Vec3(-0.5, 0.5, -0.5),
            Vec3(-0.5, -0.5, -0.5),

            Vec3(0.5, -0.5, -0.5),
            Vec3(0.5, 0.5, -0.5),
            Vec3(0.5, 0.5, 0.5),
            Vec3(0.5, -0.5, 0.5)
        };

        static const Vec3 normals[] = {
            Vec3(0, 0, 1),
            Vec3(0, 0, 1),
            Vec3(0, 0, 1),
            Vec3(0, 0, 1),

            Vec3(0, 1, 0),
            Vec3(0, 1, 0),
            Vec3(0, 0, -1),
            Vec3(0, 0, -1),

            Vec3(0, 1, 0),
            Vec3(0, 1, 0),
            Vec3(0, 0, -1),
            Vec3(0, 0, -1),

            Vec3(0, -1, 0),
            Vec3(0, -1, 0),
            Vec3(0, -1, 0),
            Vec3(0, -1, 0),

            Vec3(-1, 0, 0),
            Vec3(-1, 0, 0),
            Vec3(-1, 0, 0),
            Vec3(-1, 0, 0),

            Vec3(1, 0, 0),
            Vec3(1, 0, 0),
            Vec3(1, 0, 0),
            Vec3(1, 0, 0)
        };

        static const Vec4 tangents[] = {
            Vec4(-1, 0, 0, -1),
            Vec4(-1, 0, 0, -1),
            Vec4(-1, 0, 0, -1),
            Vec4(-1, 0, 0, -1),

            Vec4(-1, 0, 0, -1),
            Vec4(-1, 0, 0, -1),
            Vec4(-1, 0, 0, -1),
            Vec4(-1, 0, 0, -1),

            Vec4(-1, 0, 0, -1),
            Vec4(-1, 0, 0, -1),
            Vec4(-1, 0, 0, -1),
            Vec4(-1, 0, 0, -1),

            Vec4(-1, 0, 0, -1),
            Vec4(-1, 0, 0, -1),
            Vec4(-1, 0, 0, -1),
            Vec4(-1, 0, 0, -1),

            Vec4(0, 0, -1, -1),
            Vec4(0, 0, -1, -1),
            Vec4(0, 0, -1, -1),
            Vec4(0, 0, -1, -1),

            Vec4(0, 0, 1, -1),
            Vec4(0, 0, 1, -1),
            Vec4(0, 0, 1, -1),
            Vec4(0, 0, 1, -1),
        };

        static const Vec2 uvCoords[] = {
            Vec2(0, 0),
            Vec2(1, 0),
            Vec2(0, 1),
            Vec2(1, 1),

            Vec2(0, 1),
            Vec2(1, 1),
            Vec2(0, 1),
            Vec2(1, 1),

            Vec2(0, 0),
            Vec2(1, 0),
            Vec2(0, 0),
            Vec2(1, 0),

            Vec2(0, 0),
            Vec2(0, 1),
            Vec2(1, 1),
            Vec2(1, 0),

            Vec2(0, 0),
            Vec2(0, 1),
            Vec2(1, 1),
            Vec2(1, 0),

            Vec2(0, 0),
            Vec2(0, 1),
            Vec2(1, 1),
            Vec2(1, 0)
        };

        static const u16 indices[] = {
            0, 2, 3,
            0, 3, 1,
            8, 4, 5,
            8, 5, 9,
            10, 6, 7,
            10, 7, 11,
            12, 13, 14,
            12, 14, 15,
            16, 17, 18,
            16, 18, 19,
            20, 21, 22,
            20, 22, 23
        };

        if (outer == nullptr)
        {
            outer = GetTransientPackage(MODULE_NAME);
        }

        ModelLodAsset* modelLodAsset = CreateObject<ModelLodAsset>(outer, "CubeModelAsset");

        modelLodAsset->numVertices = COUNTOF(vertices);
        modelLodAsset->positionsData.LoadData(vertices, sizeof(vertices));
        modelLodAsset->normalData.LoadData(normals, sizeof(normals));
        modelLodAsset->tangentData.LoadData(tangents, sizeof(tangents));
        modelLodAsset->uv0Data.LoadData(uvCoords, sizeof(uvCoords));

        modelLodAsset->subMeshes.Resize(1);
        modelLodAsset->subMeshes[0].indexFormat = IndexFormat::Uint16;
        modelLodAsset->subMeshes[0].materialIndex = 0;
        modelLodAsset->subMeshes[0].numIndices = COUNTOF(indices);
        modelLodAsset->subMeshes[0].indicesData.LoadData(indices, sizeof(indices));

        return modelLodAsset;
    }
} // namespace CE::RPI

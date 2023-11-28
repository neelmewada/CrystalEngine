#include "System.h"

namespace CE
{
    SYSTEM_API SIZE_T GetVertexInputTypeSize(VertexInputAttribute input)
    {
        switch (input) {
            case VertexInputAttribute::Position:
                return sizeof(Vec3);
            case VertexInputAttribute::UV0:
                return sizeof(Vec2);
            case VertexInputAttribute::Normal:
                return sizeof(Vec3);
            case VertexInputAttribute::Tangent:
                return sizeof(Vec3);
            case VertexInputAttribute::Color:
                return sizeof(Vec4);
            default:
                return 0;
        }
        
        return 0;
    }

	void SubMesh::Clear()
	{
		materialSlotIndex = 0;
		indices.Clear();
	}

	void SubMesh::Release()
	{
		
	}

    void Mesh::Clear()
    {
		vertices.Clear();
		uvCoords.Clear();
		normals.Clear();
		tangents.Clear();
		subMeshes.Clear();
		vertexColors.Clear();
		vertexBuffers.Clear();
		indexBuffer = nullptr;
    }

    Mesh::~Mesh()
    {
        for (auto [flags, buffer] : vertexBuffers)
        {
            RHI::gDynamicRHI->DestroyBuffer(buffer);
        }

		if (indexBuffer != nullptr)
		{
			RHI::gDynamicRHI->DestroyBuffer(indexBuffer);
		}
        
        Clear();
    }
    
    RHI::Buffer* Mesh::GetOrCreateBuffer(const Array<VertexInputAttribute>& inputs)
    {
        RHI::BufferDesc desc{};
        desc.name = "VertexBuffer";
        desc.allocMode = RHI::BufferAllocMode::GpuMemory;
        desc.bindFlags = RHI::BufferBindFlags::VertexBuffer;
        desc.usageFlags = RHI::BufferUsageFlags::Default;
        
        desc.structureByteStride = 0;
        VertexInputAttribute flags = VertexInputAttribute::None;
        
        for (VertexInputAttribute inputType : inputs)
        {
            desc.structureByteStride += GetVertexInputTypeSize(inputType);
            flags |= inputType;
        }

		if (vertexBuffers.KeyExists(flags) && vertexBuffers[flags] != nullptr)
		{
			return vertexBuffers[flags];
		}
        
        u64 numVertices = vertices.GetSize();
        desc.bufferSize = (u64)desc.structureByteStride * numVertices;
        
        RHI::Buffer* buffer = RHI::gDynamicRHI->CreateBuffer(desc);
        
        f32* data = new f32[desc.bufferSize];
        
        for (u32 vertIdx = 0; vertIdx < numVertices; vertIdx++)
        {
			u8* posStart = (u8*)data + vertIdx * (u32)desc.structureByteStride;
            
            u8* pos = (u8*)posStart;
            
            for (VertexInputAttribute inputType : inputs)
            {
                SIZE_T size = GetVertexInputTypeSize(inputType);
                Vec2* vec2 = (Vec2*)pos;
                Vec3* vec3 = (Vec3*)pos;
                Vec4* vec4 = (Vec4*)pos;
                Color* color = (Color*)pos;
                
                switch (inputType) {
                    case VertexInputAttribute::None:
                        break;
                    case VertexInputAttribute::Position:
                        *vec3 = vertices[vertIdx];
                        break;
                    case VertexInputAttribute::UV0:
                        *vec2 = uvCoords[vertIdx];
                        break;
                    case VertexInputAttribute::Normal:
                        *vec3 = normals[vertIdx];
                        break;
                    case VertexInputAttribute::Tangent:
                        *vec3 = tangents[vertIdx];
                        break;
                    case VertexInputAttribute::Color:
                        *color = vertexColors[vertIdx];
                        break;
                }
                
				pos += size;
            }
        }
        
        RHI::BufferData uploadData{};
        uploadData.data = data;
        uploadData.dataSize = buffer->GetBufferSize();
        uploadData.startOffsetInBuffer = 0;
        buffer->UploadData(uploadData);
        
        delete[] data;
        
        if (vertexBuffers[flags] == nullptr)
            vertexBuffers[flags] = buffer;
        
        return buffer;
    }

    void Mesh::PushToBuffer(RHI::Buffer* buffer, const Array<VertexInputAttribute>& inputs)
    {
        u64 structureSize = 0;
        
        for (VertexInputAttribute inputType : inputs)
        {
            structureSize += GetVertexInputTypeSize(inputType);
        }
        
        u64 numVertices = vertices.GetSize();
        u64 bufferSize = (u64)structureSize * numVertices;
        
        if (buffer->GetBufferSize() != bufferSize)
        {
            buffer->Resize(bufferSize);
        }
        
        f32* data = new f32[bufferSize];
        
        for (u32 vertIdx = 0; vertIdx < numVertices; vertIdx++)
        {
            f32* posStart = data + vertIdx * (u32)structureSize;
            
            f32* pos = posStart;
            
            for (VertexInputAttribute inputType : inputs)
            {
                SIZE_T size = GetVertexInputTypeSize(inputType);
                Vec2* vec2 = (Vec2*)pos;
                Vec3* vec3 = (Vec3*)pos;
                Vec4* vec4 = (Vec4*)pos;
                Color* color = (Color*)pos;
                
                switch (inputType) {
                    case VertexInputAttribute::None:
                        break;
                    case VertexInputAttribute::Position:
                        *vec3 = vertices[vertIdx];
                        break;
                    case VertexInputAttribute::UV0:
                        *vec2 = uvCoords[vertIdx];
                        break;
                    case VertexInputAttribute::Normal:
                        *vec3 = normals[vertIdx];
                        break;
                    case VertexInputAttribute::Tangent:
                        *vec3 = tangents[vertIdx];
                        break;
                    case VertexInputAttribute::Color:
                        *color = vertexColors[vertIdx];
                        break;
                }
                
                pos += size;
            }
        }
        
        RHI::BufferData uploadData{};
        uploadData.data = data;
        uploadData.dataSize = buffer->GetBufferSize();
        uploadData.startOffsetInBuffer = 0;
        buffer->UploadData(uploadData);
        
        delete[] data;
        data = nullptr;
    }

	RHI::Buffer* Mesh::GetOrCreateIndexBuffer()
	{
		if (indexBuffer != nullptr)
			return indexBuffer;

		RHI::BufferDesc indexBufferDesc{};
		indexBufferDesc.name = "Index Buffer";
		indexBufferDesc.bindFlags = RHI::BufferBindFlags::IndexBuffer;
		indexBufferDesc.allocMode = RHI::BufferAllocMode::GpuMemory;
		indexBufferDesc.usageFlags = RHI::BufferUsageFlags::Default;

		constexpr SIZE_T u16Max = NumericLimits<u16>::Max();
		uses32BitIndex = vertices.GetSize() > u16Max;

		u32 totalIndices = 0;
		for (const auto& subMesh : subMeshes)
		{
			totalIndices += subMesh.indices.GetSize();
		}

		if (totalIndices == 0)
			return nullptr;

		void* bytes = nullptr;

		if (uses32BitIndex)
		{
			indexBufferDesc.bufferSize = sizeof(u32) * totalIndices;
			bytes = Memory::Malloc(sizeof(u32) * totalIndices);
			u32* data = (u32*)bytes;

			for (int i = 0; i < subMeshes.GetSize(); i++)
			{
				memcpy(data, subMeshes[i].indices.GetData(), subMeshes[i].indices.GetSize());
				data = data + subMeshes[i].indices.GetSize();
			}
		}
		else
		{
			indexBufferDesc.bufferSize = sizeof(u16) * totalIndices;
			bytes = Memory::Malloc(sizeof(u16) * totalIndices);
			u16* data = (u16*)bytes;

			for (const auto& subMesh : subMeshes)
			{
				for (int i = 0; i < subMesh.indices.GetSize(); i++)
				{
					data[i] = (u16)subMesh.indices[i];
				}

				data += subMesh.indices.GetSize();
			}
		}

		RHI::BufferData initialData{};
		initialData.data = bytes;
		initialData.dataSize = indexBufferDesc.bufferSize;
		initialData.startOffsetInBuffer = 0;

		indexBufferDesc.initialData = &initialData;

		indexBuffer = RHI::gDynamicRHI->CreateBuffer(indexBufferDesc);

		Memory::Free(bytes);

		return indexBuffer;
	}

    ModelData::~ModelData()
    {
        
    }

	ModelData* ModelData::GetCubeModel()
	{
		Package* transient = ModuleManager::Get().GetLoadedModuleTransientPackage(MODULE_NAME);
		ModelData* cubeModel = nullptr;
		if (transient != nullptr)
		{
			for (Object* object : transient->GetSubObjectMap())
			{
				if (object != nullptr && object->IsOfType<ModelData>() && object->GetName() == "CubeMesh")
				{
					cubeModel = Object::CastTo<ModelData>(object);
					break;
				}
			}
		}

		if (!cubeModel)
		{
			cubeModel = CreateObject<ModelData>(transient, "CubeMesh", OF_Transient);
			cubeModel->lod.Resize(1);
            
            cubeModel->lod[0] = CreateObject<Mesh>(cubeModel, TEXT("CubeMesh"), OF_Transient);
            
			Mesh* mesh = cubeModel->lod[0];

			mesh->subMeshes.Resize(1);
			SubMesh& submesh = mesh->subMeshes[0];
            
			mesh->vertices = {
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

			submesh.indices = {
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

			mesh->uvCoords = {
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

			mesh->normals = {
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

			mesh->tangents = {
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
		}

		return cubeModel;
	}

} // namespace CE

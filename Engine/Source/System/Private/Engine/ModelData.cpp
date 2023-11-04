#include "System.h"

namespace CE
{
    SYSTEM_API SIZE_T GetVertexInputTypeSize(VertexInputType input)
    {
        switch (input) {
            case VertexInputType::Position:
                return sizeof(Vec3);
            case VertexInputType::UV0:
                return sizeof(Vec2);
            case VertexInputType::Normal:
                return sizeof(Vec3);
            case VertexInputType::Tangent:
                return sizeof(Vec3);
            case VertexInputType::Color:
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
		submeshes.Clear();
		vertexColors.Clear();
    }

    Mesh::~Mesh()
    {
        for (auto [flags, buffer] : vertexBuffers)
        {
            RHI::gDynamicRHI->DestroyBuffer(buffer);
        }
        
        Clear();
    }
    
    RHI::Buffer* Mesh::CreateBuffer(const Array<VertexInputType>& inputs)
    {
        RHI::BufferDesc desc{};
        desc.name = "VertexBuffer";
        desc.allocMode = RHI::BufferAllocMode::GpuMemory;
        desc.bindFlags = RHI::BufferBindFlags::VertexBuffer;
        desc.usageFlags = RHI::BufferUsageFlags::Default;
        
        desc.structureByteStride = 0;
        VertexInputType flags = VertexInputType::None;
        
        for (VertexInputType inputType : inputs)
        {
            desc.structureByteStride += GetVertexInputTypeSize(inputType);
            flags |= inputType;
        }
        
        u64 numVertices = vertices.GetSize();
        desc.bufferSize = (u64)desc.structureByteStride * numVertices;
        
        RHI::Buffer* buffer = RHI::gDynamicRHI->CreateBuffer(desc);
        
        f32* data = new f32[desc.bufferSize];
        
        for (u32 vertIdx = 0; vertIdx < numVertices; vertIdx++)
        {
            f32* posStart = data + vertIdx * (u32)desc.structureByteStride;
            
            f32* pos = posStart;
            
            for (VertexInputType inputType : inputs)
            {
                SIZE_T size = GetVertexInputTypeSize(inputType);
                Vec2* vec2 = (Vec2*)pos;
                Vec3* vec3 = (Vec3*)pos;
                Vec4* vec4 = (Vec4*)pos;
                Color* color = (Color*)pos;
                
                switch (inputType) {
                    case VertexInputType::None:
                        break;
                    case VertexInputType::Position:
                        *vec3 = vertices[vertIdx];
                        break;
                    case VertexInputType::UV0:
                        *vec2 = uvCoords[vertIdx];
                        break;
                    case VertexInputType::Normal:
                        *vec3 = normals[vertIdx];
                        break;
                    case VertexInputType::Tangent:
                        *vec3 = tangents[vertIdx];
                        break;
                    case VertexInputType::Color:
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

    void Mesh::PushToBuffer(RHI::Buffer* buffer, const Array<VertexInputType>& inputs)
    {
        u64 structureSize = 0;
        
        for (VertexInputType inputType : inputs)
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
            
            for (VertexInputType inputType : inputs)
            {
                SIZE_T size = GetVertexInputTypeSize(inputType);
                Vec2* vec2 = (Vec2*)pos;
                Vec3* vec3 = (Vec3*)pos;
                Vec4* vec4 = (Vec4*)pos;
                Color* color = (Color*)pos;
                
                switch (inputType) {
                    case VertexInputType::None:
                        break;
                    case VertexInputType::Position:
                        *vec3 = vertices[vertIdx];
                        break;
                    case VertexInputType::UV0:
                        *vec2 = uvCoords[vertIdx];
                        break;
                    case VertexInputType::Normal:
                        *vec3 = normals[vertIdx];
                        break;
                    case VertexInputType::Tangent:
                        *vec3 = tangents[vertIdx];
                        break;
                    case VertexInputType::Color:
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

			mesh->submeshes.Resize(1);
			SubMesh& submesh = mesh->submeshes[0];
            
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

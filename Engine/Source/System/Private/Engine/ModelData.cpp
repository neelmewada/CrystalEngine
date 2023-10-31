#include "System.h"

namespace CE
{
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

    void Mesh::Release()
    {
        Clear();
    }
    
    RHI::Buffer* Mesh::CreateBuffer()
    {
        RHI::BufferDesc desc{};
        desc.name = "VertexBuffer";
        desc.allocMode = RHI::BufferAllocMode::GpuMemory;
        desc.bindFlags = RHI::BufferBindFlags::VertexBuffer;
        desc.usageFlags = RHI::BufferUsageFlags::Default;
        // VertexPosition + UV + Normal
        desc.structureByteStride = sizeof(Vec3) + sizeof(Vec2) + sizeof(Vec3);
        u64 numVertices = vertices.GetSize();
        desc.bufferSize = (u64)desc.structureByteStride * numVertices;
        
        RHI::Buffer* buffer = RHI::gDynamicRHI->CreateBuffer(desc);
        
        
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
			Mesh& mesh = cubeModel->lod[0];

			mesh.submeshes.Resize(1);
			SubMesh& submesh = mesh.submeshes[0];

			mesh.vertices = {
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

			mesh.uvCoords = {
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

			mesh.normals = {
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

			mesh.tangents = {
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

#include "System.h"

namespace CE
{

    StaticMesh::StaticMesh()
    {
        modelData = nullptr;
		vertexAttribs = VertexInputAttribute::Position | VertexInputAttribute::UV0 | VertexInputAttribute::Normal;
    }

    RHI::Buffer* StaticMesh::GetVertexBuffer(int lod)
    {
        if (modelData == nullptr || lod >= modelData->lod.GetSize() || lod < 0)
            return nullptr;
        
		return modelData->lod[lod]->GetOrCreateBuffer();
    }

	RHI::Buffer* StaticMesh::GetErrorShaderVertexBuffer()
	{
		if (modelData == nullptr || modelData->lod.GetSize() == 0)
			return nullptr;

		return modelData->lod[0]->GetOrCreateBuffer({ VertexInputAttribute::Position });
	}

	RHI::Buffer* StaticMesh::GetIndexBuffer(int lod)
	{
		if (modelData == nullptr || lod >= modelData->lod.GetSize() || lod < 0)
			return nullptr;

		return modelData->lod[lod]->GetOrCreateIndexBuffer();
	}

	bool StaticMesh::Uses32BitIndices(int lod)
	{
		if (modelData == nullptr || lod >= modelData->lod.GetSize() || lod < 0)
			return false;
		return modelData->lod[lod]->uses32BitIndex;
	}

	StaticMesh* StaticMesh::GetCubeMesh()
	{
		Package* transient = ModuleManager::Get().GetLoadedModuleTransientPackage(MODULE_NAME);
		ModelData* cubeModel = ModelData::GetCubeModel();
		StaticMesh* cubeMesh = nullptr;
		if (!cubeModel)
			return nullptr;

		if (transient != nullptr)
		{
			for (Object* object : transient->GetSubObjectMap())
			{
				if (object != nullptr && object->IsOfType<StaticMesh>() && object->GetName() == "CubeStaticMesh")
				{
					cubeMesh = Object::CastTo<StaticMesh>(object);
					break;
				}
			}
		}

		if (!cubeMesh)
		{
			cubeMesh = CreateObject<StaticMesh>(transient, TEXT("CubeStaticMesh"));
			cubeMesh->modelData = cubeModel;
		}

		return cubeMesh;
	}

} // namespace CE

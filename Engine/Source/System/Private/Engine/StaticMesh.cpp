#include "System.h"

namespace CE
{

    StaticMesh::StaticMesh()
    {
		//modelData = CreateDefaultSubobject<ModelData>("ModelData");
        modelData = nullptr;
    }

    RHI::Buffer* StaticMesh::GetVertexBuffer()
    {
        return vertexBuffer;
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

#include "System.h"

namespace CE
{

    StaticMeshComponent::StaticMeshComponent()
    {
		canTick = true;
    }

    void StaticMeshComponent::SetStaticMesh(StaticMesh* mesh)
    {
        this->staticMesh = mesh;
        if (mesh == nullptr || mesh->GetModelData() == nullptr)
        {
            materials.Resize(0);
            return;
        }
        
        Mesh* meshRef = mesh->GetModelData()->lod[0];
        
        materials.Resize(meshRef->GetSubMeshCount());
    }

} // namespace CE

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
        Mesh& meshRef = mesh->GetModelData()->lod[0];
        
        materials.Resize(meshRef.GetSubMeshCount());
    }

} // namespace CE

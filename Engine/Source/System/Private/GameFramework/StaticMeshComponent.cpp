#include "System.h"

namespace CE
{

    StaticMeshComponent::StaticMeshComponent()
    {
        
    }

    void StaticMeshComponent::SetStaticMesh(StaticMesh* mesh)
    {
        this->staticMesh = mesh;
        Mesh& meshRef = mesh->GetModelData()->lod[0];
        
        Super::materials.Resize(meshRef.GetSubMeshCount());
    }

} // namespace CE

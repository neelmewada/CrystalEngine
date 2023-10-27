#include "System.h"

namespace CE
{

    MeshComponent::MeshComponent()
    {

    }

    void MeshComponent::SetMaterialAt(u32 index, MaterialInterface* material)
    {
        if (index >= materials.GetSize())
            return;
        
        materials[index] = material;
    }

} // namespace CE

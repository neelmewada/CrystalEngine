#include "System.h"

namespace CE
{

    MeshComponent::MeshComponent()
    {
		canTick = true;
    }

    MeshComponent::~MeshComponent()
    {
		
    }

    void MeshComponent::SetMaterial(MaterialInterface* material, u32 subMeshIndex)
    {
        if (subMeshIndex > RPI::Limits::MaxSubMeshCount)
            return;

        if (materialsPerLod.GetSize() != GetLodCount())
        {
            materialsPerLod.Resize(GetLodCount());
        }

        for (LodMaterial& lodMaterial : materialsPerLod)
        {
            if (subMeshIndex >= lodMaterial.materials.GetSize())
            {
                lodMaterial.materials.Resize(subMeshIndex + 1);
            }

            lodMaterial.materials[subMeshIndex] = material;
        }
    }

    void MeshComponent::SetMaterial(MaterialInterface* material, u32 lodIndex, u32 subMeshIndex)
    {
        if (subMeshIndex > RPI::Limits::MaxSubMeshCount)
            return;

        if (lodIndex >= GetLodCount())
            return;

        if (materialsPerLod.GetSize() != GetLodCount())
        {
            materialsPerLod.Resize(GetLodCount());
        }

        if (materialsPerLod[lodIndex].materials.GetSize() <= subMeshIndex)
        {
            materialsPerLod[lodIndex].materials.Resize(subMeshIndex + 1);
        }

        materialsPerLod[lodIndex].materials[subMeshIndex] = material;
    }

} // namespace CE

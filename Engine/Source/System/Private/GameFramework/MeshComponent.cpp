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

    RPI::CustomMaterialMap MeshComponent::GetRpiMaterialMap()
    {
        CustomMaterialMap materialMap{};

        for (int lodIndex = 0; lodIndex < GetLodCount(); ++lodIndex)
        {
	        for (int meshIndex = 0; meshIndex < materialsPerLod[lodIndex].materials.GetSize(); ++meshIndex)
	        {
                MaterialInterface* materialInterface = materialsPerLod[lodIndex].materials[meshIndex];
                if (materialInterface == nullptr)
                    continue;
                RPI::Material* material = materialInterface->GetRpiMaterial();
                if (material == nullptr)
                    continue;

                CustomMaterialId materialId = CustomMaterialId(lodIndex, meshIndex);
                materialMap[materialId] = material;
	        }
        }

        return materialMap;
    }

    MaterialInterface* MeshComponent::GetMaterial(u32 subMeshIndex)
    {
        return GetMaterial(0, subMeshIndex);
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

    MaterialInterface* MeshComponent::GetMaterial(u32 lodIndex, u32 subMeshIndex)
    {
        if (subMeshIndex > RPI::Limits::MaxSubMeshCount)
            return nullptr;

        if (lodIndex >= GetLodCount() || lodIndex >= materialsPerLod.GetSize())
            return nullptr;

        if (subMeshIndex >= materialsPerLod[lodIndex].materials.GetSize())
            return nullptr;

        return materialsPerLod[lodIndex].materials[subMeshIndex];
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

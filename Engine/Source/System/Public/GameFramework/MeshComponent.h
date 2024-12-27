#pragma once

namespace CE
{
	class MaterialInterface;

	STRUCT()
    struct SYSTEM_API LodMaterial final
    {
        CE_STRUCT(LodMaterial)
    public:

        //! @brief Materials per sub-mesh.
        FIELD()
        Array<Ref<MaterialInterface>> materials{};
    };

	CLASS(Abstract)
	class SYSTEM_API MeshComponent : public GeometryComponent
	{
		CE_CLASS(MeshComponent, GeometryComponent)
	public:

		MeshComponent();
		virtual ~MeshComponent();

		virtual u32 GetLodCount() const { return 1; }

		virtual u32 GetLodSubMeshCount(u32 lodIndex) { return 1; }

		RPI::CustomMaterialMap GetRpiMaterialMap();

		Ref<MaterialInterface> GetMaterial(u32 subMeshIndex = 0);

		void SetMaterial(MaterialInterface* material, u32 subMeshIndex = 0);

		Ref<MaterialInterface> GetMaterial(u32 lodIndex, u32 subMeshIndex);

		void SetMaterial(MaterialInterface* material, u32 lodIndex, u32 subMeshIndex);

	protected:

		//! @brief Materials per LOD mesh
		FIELD(EditAnywhere, Category = "Materials")
        Array<LodMaterial> materialsPerLod{};

	};
    
} // namespace CE

#include "MeshComponent.rtti.h"

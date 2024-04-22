#pragma once

namespace CE
{
	class MaterialInterface;

	STRUCT()
    struct SYSTEM_API LodMaterial
    {
        CE_STRUCT(LodMaterial)
    public:

        //! @brief Materials per sub-mesh.
        FIELD()
        Array<MaterialInterface*> materials{};
    };

	CLASS(Abstract)
	class SYSTEM_API MeshComponent : public GeometryComponent
	{
		CE_CLASS(MeshComponent, GeometryComponent)
	public:

		MeshComponent();
		virtual ~MeshComponent();

		virtual u32 GetLodCount() const { return 1; }

		void SetMaterial(MaterialInterface* material, u32 subMeshIndex = 0);

		void SetMaterial(MaterialInterface* material, u32 lodIndex, u32 subMeshIndex);

	protected:

		FIELD()
        Array<LodMaterial> materialsPerLod{};

	};
    
} // namespace CE

#include "MeshComponent.rtti.h"

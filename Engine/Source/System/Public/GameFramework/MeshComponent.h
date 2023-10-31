#pragma once

namespace CE
{
	class MaterialInterface;

	CLASS(Abstract)
	class SYSTEM_API MeshComponent : public GeometryComponent
	{
		CE_CLASS(MeshComponent, GeometryComponent)
	public:

		MeshComponent();
		virtual ~MeshComponent();
        
        inline u32 GetMaterialCount() const { return materials.GetSize(); }
        
        void SetMaterialAt(u32 index, MaterialInterface* material);

	protected:

		FIELD()
		Array<MaterialInterface*> materials{};

		RHI::Buffer* vertexBuffer = nullptr;

	};
    
} // namespace CE

#include "MeshComponent.rtti.h"

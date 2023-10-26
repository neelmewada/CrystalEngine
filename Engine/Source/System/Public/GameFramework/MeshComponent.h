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

	protected:

		FIELD()
		Array<MaterialInterface*> materials{};
	};
    
} // namespace CE

#include "MeshComponent.rtti.h"
#pragma once

namespace CE
{
	CLASS(Abstract)
	class SYSTEM_API MeshComponent : public GeometryComponent
	{
		CE_CLASS(MeshComponent, GeometryComponent)
	public:

		MeshComponent();

	protected:


	};
    
} // namespace CE

#include "MeshComponent.rtti.h"
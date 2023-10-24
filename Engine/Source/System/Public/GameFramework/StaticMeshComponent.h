#pragma once

namespace CE
{
    CLASS()
	class SYSTEM_API StaticMeshComponent : public MeshComponent
	{
		CE_CLASS(StaticMeshComponent, MeshComponent)
	public:

		StaticMeshComponent();

	protected:

	};

} // namespace CE

#include "StaticMeshComponent.rtti.h"

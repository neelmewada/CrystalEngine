#pragma once

namespace CE
{
    class StaticMesh;

    CLASS()
	class SYSTEM_API StaticMeshComponent : public MeshComponent
	{
		CE_CLASS(StaticMeshComponent, MeshComponent)
	public:

		StaticMeshComponent();

        inline StaticMesh* GetStaticMesh() const { return staticMesh; }
        
        inline void SetStaticMesh(StaticMesh* mesh) { this->staticMesh = mesh; }
        
	protected:
        
        FIELD()
        StaticMesh* staticMesh = nullptr;

	};

} // namespace CE

#include "StaticMeshComponent.rtti.h"

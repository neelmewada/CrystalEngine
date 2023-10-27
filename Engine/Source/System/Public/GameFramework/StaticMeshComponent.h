#pragma once

namespace CE
{
    class StaticMesh;
	class ModelData;

    CLASS()
	class SYSTEM_API StaticMeshComponent : public MeshComponent
	{
		CE_CLASS(StaticMeshComponent, MeshComponent)
	public:

		StaticMeshComponent();

        inline StaticMesh* GetStaticMesh() const { return staticMesh; }
        
        void SetStaticMesh(StaticMesh* mesh);
        
	protected:
        
        FIELD()
        StaticMesh* staticMesh = nullptr;

	};

} // namespace CE

#include "StaticMeshComponent.rtti.h"

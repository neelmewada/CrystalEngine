#pragma once

namespace CE
{

    CLASS()
	class SYSTEM_API StaticMeshComponent : public MeshComponent
	{
		CE_CLASS(StaticMeshComponent, MeshComponent)
	public:

		StaticMeshComponent();

		~StaticMeshComponent();

		virtual u32 GetLodCount() const override;

		void SetStaticMesh(StaticMesh* staticMesh);
        
	protected:

		void Tick(f32 delta) override;

		FIELD()
		StaticMesh* staticMesh = nullptr;

		RPI::ModelHandle meshHandle{};

		b8 meshChanged = false;
	};

} // namespace CE

#include "StaticMeshComponent.rtti.h"

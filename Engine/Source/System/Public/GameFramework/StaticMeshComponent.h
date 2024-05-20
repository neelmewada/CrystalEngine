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

		virtual u32 GetLodSubMeshCount(u32 lodIndex) override;

		void SetStaticMesh(StaticMesh* staticMesh);
        
	protected:

		void OnEnabled() override;

		void OnDisabled() override;

		void Tick(f32 delta) override;

		FIELD(EditAnywhere, Category = "Static Mesh")
		StaticMesh* staticMesh = nullptr;

		RPI::ModelHandle meshHandle{};

		b8 meshChanged = false;
	};

} // namespace CE

#include "StaticMeshComponent.rtti.h"

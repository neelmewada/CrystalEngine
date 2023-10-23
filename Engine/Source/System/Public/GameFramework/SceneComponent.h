#pragma once

namespace CE
{
    CLASS()
	class SYSTEM_API SceneComponent : public ActorComponent
	{
		CE_CLASS(SystemComponent, ActorComponent)
	public:
		SceneComponent();

		// - Public API -

		bool IsSceneComponent() const override final { return true; }

		void SetupAttachment(SceneComponent* component);

		bool ComponentExists(SceneComponent* component);

	private:

		FIELD()
		Array<SceneComponent*> attachedComponents{};

		FIELD()
		SceneComponent* parentComponent = nullptr;
	};

} // namespace CE

#include "SceneComponent.rtti.h"

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

	protected:

		void SetupAttachment(SceneComponent* component);

		bool ComponentExists(SceneComponent* component);

	private:

		FIELD()
		Array<SceneComponent*> attachedComponents{};
	};

} // namespace CE

#include "SceneComponent.rtti.h"

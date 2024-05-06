#include "System.h"

namespace CE
{

	ActorComponent::ActorComponent()
	{
		canTick = true;
	}

	void ActorComponent::OnBeginPlay()
	{
		hasBegunPlaying = true;
	}

	void ActorComponent::Tick(f32 delta)
	{
		
	}

	CE::Scene* ActorComponent::GetScene() const
	{
		return owner != nullptr ? owner->GetScene() : nullptr;
	}

	bool ActorComponent::IsEnabled() const
	{
		if (!owner)
			return isEnabled;

		return isEnabled && owner->IsEnabled();
	}
} // namespace CE


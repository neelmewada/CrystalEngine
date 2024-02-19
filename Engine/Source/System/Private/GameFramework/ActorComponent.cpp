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

} // namespace CE


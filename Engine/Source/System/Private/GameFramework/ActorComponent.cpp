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

	void ActorComponent::SetEnabled(bool enabled)
	{
		if (isEnabled == enabled)
			return;

		isEnabled = enabled;

		if (isEnabled)
			OnEnabled();
		else
			OnDisabled();
	}

	void ActorComponent::OnFieldEdited(FieldType* field)
	{
		Super::OnFieldEdited(field);

		thread_local const Name isEnabledName = "isEnabled";

		if (field->GetName() == isEnabledName)
		{
			if (IsEnabled())
				OnEnabled();
			else
				OnDisabled();
		}
	}

	void ActorComponent::OnEnabled()
	{
		
	}

	void ActorComponent::OnDisabled()
	{

	}

} // namespace CE


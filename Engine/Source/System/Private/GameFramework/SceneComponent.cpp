#include "System.h"

namespace CE
{

    SceneComponent::SceneComponent()
    {
		canTick = true;
    }

	void SceneComponent::SetupAttachment(SceneComponent* component)
	{
		if (!component || component == this)
			return;
		if (ComponentExists(component))
		{
			CE_LOG(Error, All, "SceneComponent::SetupAttachment called with a scene component that already exists in it's hierarcy");
			return;
		}

		component->parentComponent = this;
		component->owner = this->owner;
		attachedComponents.Add(component);

		if (HasBegunPlaying() && !component->HasBegunPlaying())
		{
			component->OnBeginPlay();
		}

		if (GetScene() != nullptr)
		{
			Scene* scene = GetScene();
			scene->componentsByType[component->GetTypeId()][component->GetUuid()] = component;
		}
	}

	bool SceneComponent::ComponentExists(SceneComponent* component)
	{
		for (auto comp : attachedComponents)
		{
			if (comp == component || comp->ComponentExists(component))
				return true;
		}
		return false;
	}

	void SceneComponent::OnBeginPlay()
	{
		Super::OnBeginPlay();


	}

	void SceneComponent::Tick(f32 delta)
	{
		Super::Tick(delta);
        
		if (IsDirty())
		{
			auto actor = GetActor();

			localTranslationMat[0][3] = localPosition.x;
			localTranslationMat[1][3] = localPosition.y;
			localTranslationMat[2][3] = localPosition.z;

			localRotation = Quat::EulerDegrees(localEulerAngles);
			localRotationMat = localRotation.ToMatrix();
			
			localScaleMat[0][0] = localScale.x;
			localScaleMat[1][1] = localScale.y;
			localScaleMat[2][2] = localScale.z;

			localTransform = localTranslationMat * localRotationMat * localScaleMat;

			if (parentComponent != nullptr)
			{
				transform = parentComponent->transform * localTransform;
			}
			else if (actor != nullptr && actor->parent != nullptr)
			{
				auto parent = actor->parent;
				while (parent != nullptr)
				{
					if (parent->rootComponent != nullptr)
						break;
					if (parent->GetParentActor() == nullptr)
						break;
					parent = parent->GetParentActor();
				}

				if (parent && parent->rootComponent != nullptr)
					transform = parent->rootComponent->transform * localTransform;
				else
					transform = localTransform;
			}
			else
			{
				transform = localTransform;
			}

			isDirty = false;
		}

		for (auto component : attachedComponents)
		{
			component->Tick(delta);
		}
	}

	bool SceneComponent::IsDirty()
	{
		if (isDirty)
			return true;

		Actor* actor = GetActor();

		if (parentComponent != nullptr && parentComponent->IsDirty())
			return true;

		if (parentComponent == nullptr)
		{
			auto parentActor = actor->GetParentActor();

			if (actor != nullptr && parentActor != nullptr && parentActor->rootComponent != nullptr)
			{
				return parentActor->rootComponent->IsDirty();
			}
			return false;
		}

		return false;
	}

	void SceneComponent::SetDirty(bool set)
	{
		isDirty = set;
	}

} // namespace CE


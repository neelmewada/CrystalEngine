#include "System.h"

namespace CE
{

	Scene::Scene()
	{
		root = CreateDefaultSubobject<Actor>("SceneRoot");
		root->scene = this;
	}

	Scene::~Scene()
	{
        
	}

	void Scene::Tick(f32 delta)
	{
		if (root != nullptr)
		{
			root->Tick(delta);
		}
	}

	void Scene::OnActorChainAttached(Actor* actor)
	{
		if (!actor)
			return;

		std::function<void(Actor*)> recursivelyAdd = [&](Actor* add)
			{
				actorInstancesByUuid[add->GetUuid()] = add;

				for (auto child : add->children)
				{
					if (child != nullptr && child != add)
						recursivelyAdd(child);
				}
			};

		recursivelyAdd(actor);
	}

	void Scene::OnActorChainDetached(Actor* actor)
	{
		if (!actor)
			return;

		std::function<void(Actor*)> recursivelyRemove = [&](Actor* remove)
			{
				actorInstancesByUuid.Remove(remove->GetUuid());

				for (auto child : remove->children)
				{
					if (child != nullptr && child != remove)
						recursivelyRemove(child);
				}
			};

		recursivelyRemove(actor);
	}

} // namespace CE

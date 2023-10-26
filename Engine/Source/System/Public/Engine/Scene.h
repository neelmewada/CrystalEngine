#pragma once

namespace CE
{
	class Actor;

	CLASS()
	class SYSTEM_API Scene : public Asset
	{
		CE_CLASS(Scene, Asset)
	public:

		Scene();
		virtual ~Scene();

		inline Actor* GetRootActor() const
		{
			return root;
		}
        
		virtual void Tick(f32 delta);

	protected:

		FIELD()
		Actor* root = nullptr;
	};
    
} // namespace CE

#include "Scene.rtti.h"

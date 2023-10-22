#pragma once

namespace CE
{
	CLASS()
	class SYSTEM_API GameInstance : public Object
	{
		CE_CLASS(GameInstance, Object)
	public:

		GameInstance();

		// - Public API -

		virtual void Initialize();
		virtual void Shutdown();

	protected:

	};
    
} // namespace CE

#include "GameInstance.rtti.h"
#pragma once

namespace CE
{
	CLASS(NonSerialized)
	class GAMESYSTEM_API GameEngine : public Engine
	{
		CE_CLASS(GameEngine, Engine)
	public:

		GameEngine();
		virtual ~GameEngine();

	};
    
} // namespace CE

#include "GameEngine.rtti.h"

#include "CoreRPI.h"

namespace CE::RPI
{

    RPISystem& RPISystem::Get()
    {
		static RPISystem instance{};
        return instance;
    }

    void RPISystem::Initialize()
    {
		passSystem.Initialize();
    }

    void RPISystem::Shutdown()
    {
		passSystem.Shutdown();
    }

} // namespace CE::RPI

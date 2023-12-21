#include "CoreRPI.h"

namespace CE::RPI
{
    void RPISystem::Initialize()
    {
		passSystem.Initialize();
    }

    void RPISystem::Shutdown()
    {
		passSystem.Shutdown();
    }

} // namespace CE::RPI

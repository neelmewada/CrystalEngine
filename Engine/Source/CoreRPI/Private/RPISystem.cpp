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

    }

    void RPISystem::Shutdown()
    {

    }

} // namespace CE::RPI

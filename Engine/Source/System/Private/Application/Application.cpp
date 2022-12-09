
#include "Application/Application.h"

namespace CE
{

    Application::Application(int argc, char** argv)
    {
        ApplicationBus::BusConnect(this);
    }

    Application::~Application()
    {
        ApplicationBus::BusDisconnect(this);
    }

    void Application::RunMainLoop()
    {
        while (!bExitMainLoopRequested)
        {
            Tick();
        }
    }

    void Application::ExitMainLoop()
    {
        bExitMainLoopRequested = true;
    }

    void Application::Tick()
    {
        auto deltaTime = GetTickDeltaTime();
        Super::Tick();
    }

} // namespace CE

CE_RTTI_CLASS_IMPL(SYSTEM_API, CE, Application)


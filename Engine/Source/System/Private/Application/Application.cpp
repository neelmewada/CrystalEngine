
#include "Application/Application.h"

#include "System.h"

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
        while (!exitMainLoopRequested)
        {
            Tick();
        }
    }

    void Application::ExitMainLoop()
    {
        exitMainLoopRequested = true;
    }

    void Application::Tick()
    {
        auto deltaTime = GetTickDeltaTime();
        Super::Tick();
        
        if (GEngine != nullptr)
        {
            GEngine->Tick(deltaTime);
        }
    }

} // namespace CE

CE_RTTI_CLASS_IMPL(SYSTEM_API, CE, Application)


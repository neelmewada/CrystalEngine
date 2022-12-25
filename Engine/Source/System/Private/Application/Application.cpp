
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

        delete engineRef;
    }

    Engine* Application::GetEngineRef()
    {
        if (engineRef == nullptr)
        {
            engineRef = new Engine;
        }
        return engineRef;
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
        
        if (engineRef != nullptr)
        {
            engineRef->Tick(deltaTime);
        }
    }

} // namespace CE

CE_RTTI_CLASS_IMPL(SYSTEM_API, CE, Application)

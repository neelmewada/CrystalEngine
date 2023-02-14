
#include "Application/Application.h"

#include "System.h"

namespace CE
{

    Application::Application(int argc, char** argv)
    {
        CE_CONNECT(ApplicationBus, this);

        CE_PUBLISH(ApplicationBus, RunMainLoop);
    }

    Application::~Application()
    {
        CE_DISCONNECT(ApplicationBus, this);

        delete engineRef;
    }

    void Application::GetEngineRef(Engine** outEngineRef)
    {
        if (outEngineRef == nullptr)
            return;
        if (engineRef == nullptr)
        {
            engineRef = new Engine;
        }
        *outEngineRef = engineRef;
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


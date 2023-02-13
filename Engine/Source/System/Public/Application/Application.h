#pragma once

#include "Object/Object.h"
#include "Object/SystemObject.h"
#include "Component/ComponentApplication.h"

#include "ApplicationBus.h"

namespace CE
{
    class Engine;

    class SYSTEM_API Application
        : public ComponentApplication
    {
        CE_CLASS(Application, ComponentApplication);

    public:

        Application(int argc, char** argv);

        virtual ~Application();

        virtual void Tick() override;

    public: // ApplicationBus Events
        virtual Engine* GetEngineRef();

        virtual void RunMainLoop();
        virtual void ExitMainLoop();

    protected:
        bool exitMainLoopRequested = false;

        Engine* engineRef = nullptr;
    };
    
} // namespace CE


CE_RTTI_CLASS(SYSTEM_API, CE, Application,
    CE_SUPER(),
    CE_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST(
        CE_FUNCTION(GetEngineRef, Event)
        CE_FUNCTION(RunMainLoop, Event)
        CE_FUNCTION(ExitMainLoop, Event)
    )
)

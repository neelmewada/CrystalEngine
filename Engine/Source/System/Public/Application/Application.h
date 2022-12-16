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
        , public ApplicationBus::Handler
    {
        CE_CLASS(Application, ComponentApplication);

    public:

        Application(int argc, char** argv);

        virtual ~Application();

        virtual Engine* GetEngineRef() override;

        virtual void RunMainLoop() override;
        virtual void ExitMainLoop() override;

        virtual void Tick() override;

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
    CE_FUNCTION_LIST()
)

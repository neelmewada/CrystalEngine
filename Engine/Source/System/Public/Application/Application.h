#pragma once

#include "Object/Object.h"
#include "System/SystemObject.h"
#include "Component/ComponentApplication.h"

#include "ApplicationBus.h"

namespace CE
{

    class SYSTEM_API Application
        : public SystemObject
        , public IComponentApplication
        , public ApplicationBus::Handler
    {

    public:

        Application(int argc, char** argv);

        virtual ~Application();

        virtual void RunMainLoop() override;
        virtual void ExitMainLoop() override;

    };
    
} // namespace CE

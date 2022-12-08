#pragma once

#include "TickBus.h"
#include "ComponentApplicationBus.h"

#include <time.h>

namespace CE
{

    class CORE_API IComponentApplication
        : public ComponentApplicationBus::Handler
        , public TickRequestBus::Handler
    {

    public:

        IComponentApplication();
        IComponentApplication(int argc, char** argv);

        virtual ~IComponentApplication();

        //////////////////////////////////////////////////////////////////////////
        // ComponentApplicationRequests

        virtual IComponentApplication* GetApplication() override;


        //////////////////////////////////////////////////////////////////////////
        // TickRequests

        virtual f32 GetTickDeltaTime() override;


        //////////////////////////////////////////////////////////////////////////

        virtual void Tick();

    protected:

        clock_t PrevClock = 0;
    };
    
} // namespace CE

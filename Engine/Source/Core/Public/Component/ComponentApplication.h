#pragma once

#include "TickBus.h"
#include "ComponentApplicationBus.h"

#include "Object/SystemObject.h"

#include <time.h>

namespace CE
{

    class CORE_API ComponentApplication
        : public SystemObject
        , public ComponentApplicationBus::Handler
        , public TickRequestBus::Handler
    {
        CE_CLASS(ComponentApplication, SystemObject)

    public:

        ComponentApplication();
        ComponentApplication(int argc, char** argv);

        virtual ~ComponentApplication();

        //////////////////////////////////////////////////////////////////////////
        // ComponentApplicationRequests

        virtual ComponentApplication* GetApplication() override;


        //////////////////////////////////////////////////////////////////////////
        // TickRequests

        /// To be called before calling Tick()
        virtual f32 GetTickDeltaTime() override;


        //////////////////////////////////////////////////////////////////////////

        virtual void Tick();

        virtual void TickSystem();

    protected:
        f32 TickDeltaTime = 0;
        clock_t PrevClock = 0;
    };
    
} // namespace CE

CE_RTTI_CLASS(CORE_API, CE, ComponentApplication,
    CE_SUPER(CE::SystemObject),
    CE_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        
    ),
    CE_FUNCTION_LIST()
)


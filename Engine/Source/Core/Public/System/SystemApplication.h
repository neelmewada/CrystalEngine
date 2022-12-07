#pragma once

#include "Object/Object.h"

#include "Component/SystemComponent.h"
#include "SystemApplicationBus.h"
#include "TickBus.h"

namespace CE
{

    /// A class that can be used as a base for a component based application
    class CORE_API SystemApplication :
        public CE::SystemObject,
        public SystemApplicationBus::Handler,
        public TickRequestBus::Handler
    {
        CE_CLASS(SystemApplication, CE::SystemObject);

    public:

        SystemApplication();
        SystemApplication(int argc, char** argv);

        virtual ~SystemApplication();
        
        virtual SystemApplication* GetApplication() override { return this; }
        
    };

} // namespace CE

CE_RTTI_CLASS(CORE_API, CE, SystemApplication,
    CE_SUPER(CE::SystemObject),
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST()
)

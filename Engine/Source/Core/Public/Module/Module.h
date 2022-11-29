#pragma once

#include "Misc/CoreDefines.h"

namespace CE
{

    class CORE_API Module
    {
    public:
        Module();
        virtual ~Module();

        CE_NO_COPY(Module);

        virtual void StartupModule() = 0;
        virtual void ShutdownModule() = 0;

        virtual void RegisterTypes() {}
    };

    class DefaultModuleImpl : public Module
    {
    public:

        void StartupModule() override
        {

        }

        void ShutdownModule() override
        {

        }

    };
    
} // namespace CE



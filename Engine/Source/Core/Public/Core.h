#pragma once

#include "CoreMinimal.h"

// Module Class

namespace CE
{
    class CoreModule : public Module
    {
    public:
        void StartupModule() override;
        void ShutdownModule() override;

        void RegisterTypes() override;

    private:
        DelegateHandle onBeforeModuleUnloadHandle{};
    };
}

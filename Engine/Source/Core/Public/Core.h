#pragma once

#include "CoreMinimal.h"

// Module Class

namespace CE
{
    class CoreModule : public CE::Module
    {
    public:
        void RegisterTypes() override;
        void StartupModule() override;
        void ShutdownModule() override;

    };
}

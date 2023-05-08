#pragma once

#include "CoreMinimal.h"

namespace CE
{
    class COREGUI_API CoreGUIModule : public CE::Module
    {
    public:
        virtual void StartupModule() override;

        virtual void ShutdownModule() override;

        virtual void RegisterTypes() override;
    };
}
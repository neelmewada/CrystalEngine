#pragma once

#include "CoreMinimal.h"

namespace CE
{
    class CORERHI_API CoreRHIModule : public CE::Module
    {
    public:
        virtual void StartupModule() override;

        virtual void ShutdownModule() override;

        virtual void RegisterTypes() override;
    };
}
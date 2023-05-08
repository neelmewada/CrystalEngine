#pragma once

#include "CoreMinimal.h"

#include "RHI/RHIDefinitions.h"
#include "RHI/RHIResources.h"
#include "RHI/RHI.h"

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
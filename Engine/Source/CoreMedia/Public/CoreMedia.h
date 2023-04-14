#pragma once

#include "CoreMinimal.h"

// Media
#include "CoreMedia/Image.h"

namespace CE
{
    class COREMEDIA_API CoreMediaModule : public CE::Module
    {
    public:
        virtual void StartupModule() override;

        virtual void ShutdownModule() override;

        virtual void RegisterTypes() override;
    };
}
#pragma once

#include "CoreMinimal.h"

// Media
#include "CoreMedia/Image.h"
#include "CoreMedia/CubeMap.h"

#include "CoreMedia/ImageEncoder.h"

#include "CoreMedia/Font.h"

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
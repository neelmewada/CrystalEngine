#pragma once

#include "CoreMinimal.h"

namespace CE
{
    class %(Name_API)s %(Name)sModule : public CE::Module
    {
    public:
        virtual void StartupModule() override;

        virtual void ShutdownModule() override;

        virtual void RegisterTypes() override;
    };
}
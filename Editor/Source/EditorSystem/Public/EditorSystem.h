#pragma once

#include "CoreMinimal.h"

#include "Events/EditorSystemEventBus.h"
#include "Application/EditorQtApplication.h"

// Data
#include "Data/EditorPrefs.h"

namespace CE::Editor
{

    class EDITORSYSTEM_API EditorSystemModule : public Module
    {
    public:

        virtual void StartupModule() override;

        virtual void ShutdownModule() override;

        virtual void RegisterTypes() override;

    };
    
} // namespace CE::Editor

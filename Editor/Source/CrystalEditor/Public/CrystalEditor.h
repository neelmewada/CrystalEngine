#pragma once

#include "Events/EditorSystemEventBus.h"
#include "Application/EditorQtApplication.h"
#include "Application/CrystalEditorApplication.h"

#include "Module/ModuleManager.h"

namespace CE::Editor
{

    class CRYSTALEDITOR_API CrystalEditorModule : public Module
    {
    public:

        virtual void StartupModule() override;

        virtual void ShutdownModule() override;

        virtual void RegisterTypes() override;

    };
    
} // namespace CE::Editor

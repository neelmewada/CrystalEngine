#pragma once

#include "Launch/EditorLoop.h"

#include "Events/EditorSystemEventBus.h"
#include "Application/EditorQtApplication.h"
#include "Application/CrystalEditorApplication.h"

#include "Editor/EditorWindowBase.h"
#include "Editor/EditorViewBase.h"

// Drawers
#include "Drawers/DrawerBase.h"
#include "Drawers/FieldDrawer.h"
#include "Drawers/VectorFieldDrawer.h"
#include "Drawers/StringFieldDrawer.h"
#include "Drawers/GameComponentDrawer.h"

// Editor Buses
#include "Editor/SceneEditor/SceneOutlinerViewBus.h"

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

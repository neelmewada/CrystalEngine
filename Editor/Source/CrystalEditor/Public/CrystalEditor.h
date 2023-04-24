#pragma once

#include "EditorCore.h"

#include "Engine/EditorEngine.h"
#include "Launch/EditorLoop.h"

#include "Events/EditorSystemEventBus.h"
#include "Application/EditorQtApplication.h"
#include "Application/CrystalEditorApplication.h"

#include "Editor/EditorWindow.h"
#include "Editor/EditorView.h"

// Drawers
#include "Drawers/GameComponentDrawer.h"

// Editor Buses
#include "Editor/SceneEditor/SceneOutlinerViewBus.h"
#include "Editor/CrystalEditorBus.h"

#include "Module/ModuleManager.h"

namespace ads
{
    class CDockWidget;
}

namespace CE::Editor
{
    class CrystalEditorWindow;

    CRYSTALEDITOR_API extern EditorEngine* gEditorEngine;

    class CRYSTALEDITOR_API CrystalEditor
    {
    public:
        CrystalEditor() = delete;

        static EditorWindow* GetEditorWindow(ClassType* editorWindowType);

        static ads::CDockWidget* GetEditorWindowDockWidget(ClassType* editorWindowType);

        template<typename TEditorWindowClass>
        CE_INLINE static TEditorWindowClass* GetEditorWindow()
        {
            return static_cast<TEditorWindowClass*>(GetEditorWindow(TEditorWindowClass::Type()));
        }

        template<typename TEditorWindowClass>
        CE_INLINE static ads::CDockWidget* GetEditorWindowDockWidget()
        {
            return GetEditorWindowDockWidget(TEditorWindowClass::Type());
        }

    private:
        static CrystalEditorWindow* window;

        friend class CrystalEditorWindow;
    };

    class CRYSTALEDITOR_API CrystalEditorModule : public Module
    {
    public:

        virtual void StartupModule() override;

        virtual void ShutdownModule() override;

        virtual void RegisterTypes() override;

    };
    
} // namespace CE::Editor

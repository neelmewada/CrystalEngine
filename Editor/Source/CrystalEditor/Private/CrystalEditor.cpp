
#include "CrystalEditor.h"

// Editor Windows
#include "Editor/SceneEditor/SceneEditorWindow.h"
#include "Editor/TextureEditor/TextureEditorWindow.h"

// Scene Editor Views
#include "Editor/SceneEditor/ViewportView/ViewportView.h"
#include "Editor/SceneEditor/SceneOutlinerView/SceneOutlinerView.h"
#include "Editor/SceneEditor/DetailsView/DetailsView.h"
#include "Editor/SceneEditor/ConsoleView/ConsoleView.h"

#include "CrystalEditor.private.h"
CE_IMPLEMENT_MODULE_AUTORTTI(CrystalEditor, CE::Editor::CrystalEditorModule)

namespace CE::Editor
{
    CRYSTALEDITOR_API EditorEngine* gEditorEngine = nullptr;

    CrystalEditorWindow* CrystalEditor::window = nullptr;

    EditorWindow* CrystalEditor::GetEditorWindow(ClassType* editorWindowType)
    {
        if (window == nullptr)
            return nullptr;

        return window->GetEditorWindow(editorWindowType);
    }

    ads::CDockWidget* CrystalEditor::GetEditorWindowDockWidget(ClassType* editorWindowType)
    {
        if (window == nullptr)
			return nullptr;

        return window->GetEditorWindowDockWidget(editorWindowType);
    }

    void CrystalEditorModule::StartupModule()
    {

    }

    void CrystalEditorModule::ShutdownModule()
    {

    }

    void CrystalEditorModule::RegisterTypes()
    {
        // Editor Windows
        CE_REGISTER_TYPES(
            EditorWindow,
            SceneEditorWindow
        );

        // Scene Editor Views
        CE_REGISTER_TYPES(
            ViewportView,
            SceneOutlinerView,
            DetailsView,
            ConsoleView,
            AssetsView
        );

        CE_REGISTER_TYPES(
            GameComponentDrawer
        );

        // ********************************************
        // Register Drawers
        CE_REGISTER_DRAWER(GameComponentDrawer, GameComponent);

        // *********************************************
        // Register Editor Window Classes
        CE_REGISTER_BUILTIN_ASSET_EDITOR(SceneAsset, SceneEditorWindow); // Scene Editor
        CE_REGISTER_ASSET_EDITOR(TextureAsset, TextureEditorWindow); // Texture Editor
    }

} // namespace CE::Editor

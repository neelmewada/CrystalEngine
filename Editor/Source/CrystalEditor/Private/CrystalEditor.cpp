
#include "CrystalEditor.h"

// Editor Windows
#include "Editor/SceneEditor/SceneEditorWindow.h"

// Scene Editor Views
#include "Editor/SceneEditor/ViewportView/ViewportView.h"
#include "Editor/SceneEditor/SceneOutlinerView/SceneOutlinerView.h"
#include "Editor/SceneEditor/DetailsView/DetailsView.h"
#include "Editor/SceneEditor/ConsoleView/ConsoleView.h"

#include "CrystalEditor.private.h"
CE_IMPLEMENT_MODULE_AUTORTTI(CrystalEditor, CE::Editor::CrystalEditorModule)

namespace CE::Editor
{

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
            EditorWindowBase,
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

    }

} // namespace CE::Editor

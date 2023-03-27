
#include "CrystalEditor.h"

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
            ConsoleView
        );

        CE_REGISTER_TYPES(
            GameComponentDrawer
        );

        // Register Drawers
        CE_REGISTER_DRAWER(GameComponentDrawer, GameComponent);
        
        // Register Field Drawers
        //CE_REGISTER_FIELD_DRAWERS(VectorFieldDrawer, Vec2, Vec3, Vec4, Vec2i, Vec3i, Vec4i);
        //CE_REGISTER_FIELD_DRAWERS(StringFieldDrawer, String);
    }

} // namespace CE::Editor

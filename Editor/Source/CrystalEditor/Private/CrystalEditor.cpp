
#include "CrystalEditor.h"

#include "Editor/SceneEditor/SceneEditorWindow.h"

CE_IMPLEMENT_MODULE(CrystalEditor, CE::Editor::CrystalEditorModule)

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
        CE_REGISTER_TYPES(
            EditorWindowBase,
            SceneEditorWindow
        )
        
        // Drawer Classes
        CE_REGISTER_TYPES(
            DrawerBase,
            GameComponentDrawer,
            FieldDrawer,
            VectorFieldDrawer
        );

        // Register Drawers
        CE_REGISTER_DRAWER(GameComponentDrawer, GameComponent);
        
        // Register Field Drawers
        CE_REGISTER_FIELD_DRAWERS(VectorFieldDrawer, Vec2, Vec3, Vec4, Vec2i, Vec3i, Vec4i);
    }

} // namespace CE::Editor

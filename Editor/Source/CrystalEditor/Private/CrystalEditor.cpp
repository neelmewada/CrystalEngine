
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
        
        // Drawers
        CE_REGISTER_TYPES(
            DrawerBase
        );
    }

} // namespace CE::Editor

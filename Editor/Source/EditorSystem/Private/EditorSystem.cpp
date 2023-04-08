
#include "EditorSystem.h"

CE_IMPLEMENT_MODULE(EditorSystem, CE::Editor::EditorSystemModule)

namespace CE::Editor
{

    void EditorSystemModule::StartupModule()
    {

    }

    void EditorSystemModule::ShutdownModule()
    {

    }

    void EditorSystemModule::RegisterTypes()
    {
        CE_REGISTER_TYPES(
            ProjectHistory,
            EditorPrefs
        )
    }

} // namespace CE::Editor


#include "EditorCore.h"

namespace CE::Editor
{

    class EditorCoreModule : public Module
    {
    public:

        void StartupModule() override
        {
			EditorPrefs::Get().LoadPrefs();
        }

        void ShutdownModule() override
        {
			EditorPrefs::Get().SavePrefs();
        }

        void RegisterTypes() override
        {

        }

        void DeregisterTypes() override
        {

        }
    };

} // namespace CE

#include "EditorCore.private.h"
#include "Resource.h"

CE_IMPLEMENT_MODULE(EditorCore, CE::Editor::EditorCoreModule)


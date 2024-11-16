
#include "EditorCore.h"

namespace CE::Editor
{

    class EditorCoreModule : public Module
    {
    public:

        void StartupModule() override
        {
            
        }

        void ShutdownModule() override
        {

			if (ProjectManager::TryGet())
			{
                ProjectManager::TryGet()->BeginDestroy();
			}
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

CE_IMPLEMENT_MODULE(EditorCore, CE::Editor::EditorCoreModule)



#include "EditorCore.h"

namespace CE::Editor
{

    class EditorCoreModule : public Module
    {
    public:

        void StartupModule() override
        {
            PropertyEditorRegistry::Get().Register(GetTypeId<Array<u8>>(), GetStaticClass<ArrayPropertyEditor>());
        }

        void ShutdownModule() override
        {
            PropertyEditorRegistry::Get().Deregister(GetTypeId<Array<u8>>());

			if (ProjectManager::TryGet())
			{
                ProjectManager::TryGet()->Destroy();
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



#include "EditorSystem.h"

#include "EditorSystem.private.h"


namespace CE::Editor
{
    class EditorSystemModule : public CE::Module
    {
    public:
        virtual void StartupModule() override
        {
			gEngine = CreateObject<EditorEngine>(nullptr, "EditorEngine", OF_Transient);
        }

        virtual void ShutdownModule() override
        {
			gEngine->RequestDestroy();
			gEngine = nullptr;
        }

        virtual void RegisterTypes() override
        {

        }
    };
}

CE_IMPLEMENT_MODULE(EditorSystem, CE::Editor::EditorSystemModule)

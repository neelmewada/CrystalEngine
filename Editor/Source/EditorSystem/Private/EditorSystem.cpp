
#include "EditorSystem.h"

#include "EditorSystem.private.h"


namespace CE::Editor
{
    class EditorSystemModule : public CE::Module
    {
    public:
        virtual void StartupModule() override
        {

        }

        virtual void ShutdownModule() override
        {

        }

        virtual void RegisterTypes() override
        {

        }
    };
}

CE_IMPLEMENT_MODULE(EditorSystem, CE::Editor::EditorSystemModule)

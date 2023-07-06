
#include "EditorWidgets.h"

#include "EditorWidgets.private.h"


namespace CE::Editor
{
    class EditorWidgetsModule : public CE::Module
    {
    public:
        void StartupModule() override
        {

        }

        void ShutdownModule() override
        {

        }

        void RegisterTypes() override
        {

        }
    };
}

CE_IMPLEMENT_MODULE(EditorWidgets, CE::Editor::EditorWidgetsModule)


#include "EditorWidgets.h"

#include "EditorWidgets.private.h"


namespace CE::Editor
{
    class EditorWidgetsModule : public CE::Module
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

CE_IMPLEMENT_MODULE(EditorWidgets, CE::Editor::EditorWidgetsModule)

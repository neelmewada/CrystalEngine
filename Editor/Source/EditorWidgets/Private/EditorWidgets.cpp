
#include "EditorWidgets.h"


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

#include "EditorWidgets.private.h"
#include "Resource.h"

CE_IMPLEMENT_MODULE(EditorWidgets, CE::Editor::EditorWidgetsModule)

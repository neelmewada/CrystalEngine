
#include "EditorWidgets.h"


namespace CE::Editor
{
	using namespace CE::Widgets;

    class EditorWidgetsModule : public CE::Module
    {
    public:

        void StartupModule() override
        {
			GetStyleManager()->AddResourceSearchModule(MODULE_NAME);
        }

        void ShutdownModule() override
        {
			GetStyleManager()->RemoveResourceSearchModule(MODULE_NAME);
        }

        void RegisterTypes() override
        {

        }
    };
}

#include "EditorWidgets.private.h"
#include "Resource.h"

CE_IMPLEMENT_MODULE(EditorWidgets, CE::Editor::EditorWidgetsModule)

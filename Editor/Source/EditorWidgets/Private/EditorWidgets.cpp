
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

			onClassRegistered = CoreObjectDelegates::onClassRegistered.AddDelegateInstance(MemberDelegate(&EditorWidgetsModule::OnClassRegistered, this));
        }

        void ShutdownModule() override
        {
			CoreObjectDelegates::onClassRegistered.RemoveDelegateInstance(onClassRegistered);

			GetStyleManager()->RemoveResourceSearchModule(MODULE_NAME);
        }

        void RegisterTypes() override
        {
			
        }

		void OnClassRegistered(ClassType* classType)
		{

		}

		DelegateHandle onClassRegistered = 0;
    };
}

#include "EditorWidgets.private.h"
#include "Resource.h"

CE_IMPLEMENT_MODULE(EditorWidgets, CE::Editor::EditorWidgetsModule)

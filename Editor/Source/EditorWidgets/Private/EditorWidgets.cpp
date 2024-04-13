
#include "EditorWidgets.h"


namespace CE::Editor
{
	using namespace CE::Widgets;

    class EditorWidgetsModule : public CE::Module
    {
    public:

        void StartupModule() override
        {
			

			onClassRegistered = CoreObjectDelegates::onClassRegistered.AddDelegateInstance(MemberDelegate(&EditorWidgetsModule::OnClassRegistered, this));
			onClassDeregistered = CoreObjectDelegates::onClassDeregistered.AddDelegateInstance(MemberDelegate(&EditorWidgetsModule::OnClassDeregistered, this));
        }

        void ShutdownModule() override
        {
			CoreObjectDelegates::onClassRegistered.RemoveDelegateInstance(onClassRegistered);
			CoreObjectDelegates::onClassDeregistered.RemoveDelegateInstance(onClassDeregistered);
			onClassRegistered = onClassDeregistered = 0;

        }

        void RegisterTypes() override
        {
			
        }

		void OnClassRegistered(ClassType* classType)
		{
			if (classType == nullptr)
				return;
			
			
		}

		void OnClassDeregistered(ClassType* classType)
		{
			if (classType == nullptr)
				return;

			
		}

		DelegateHandle onClassRegistered = 0;
		DelegateHandle onClassDeregistered = 0;
    };
}

#include "EditorWidgets.private.h"
#include "Resource.h"

CE_IMPLEMENT_MODULE(EditorWidgets, CE::Editor::EditorWidgetsModule)

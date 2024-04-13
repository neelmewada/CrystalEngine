#include "CrystalEditor.h"

namespace CE
{
    class CrystalEditorModule : public CE::Module
    {
    public:
        virtual void StartupModule() override
        {
			onClassReg = CoreObjectDelegates::onClassRegistered.AddDelegateInstance(MemberDelegate(&CrystalEditorModule::OnClassRegistered, this));
			onClassDereg = CoreObjectDelegates::onClassDeregistered.AddDelegateInstance(MemberDelegate(&CrystalEditorModule::OnClassDeregistered, this));
        }

        virtual void ShutdownModule() override
        {
			CoreObjectDelegates::onClassRegistered.RemoveDelegateInstance(onClassReg);
			CoreObjectDelegates::onClassDeregistered.RemoveDelegateInstance(onClassDereg);
        }

        virtual void RegisterTypes() override
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

		DelegateHandle onClassReg = 0;
		DelegateHandle onClassDereg = 0;
    };
}

#include "CrystalEditor.private.h"

CE_IMPLEMENT_MODULE(CrystalEditor, CE::CrystalEditorModule)

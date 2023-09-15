
#include "CrystalEditor.h"

namespace CE
{
    class CrystalEditorModule : public CE::Module
    {
    public:
        virtual void StartupModule() override
        {
			GetStyleManager()->AddResourceSearchModule(MODULE_NAME);

			onClassReg = CoreObjectDelegates::onClassRegistered.AddDelegateInstance(MemberDelegate(&CrystalEditorModule::OnClassRegistered, this));
			onClassDereg = CoreObjectDelegates::onClassDeregistered.AddDelegateInstance(MemberDelegate(&CrystalEditorModule::OnClassDeregistered, this));
        }

        virtual void ShutdownModule() override
        {
			GetStyleManager()->RemoveResourceSearchModule(MODULE_NAME);

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
			
			if (classType->IsSubclassOf<EditorWindow>() && classType != EditorWindow::StaticType())
			{
				auto assetDefAttrib = classType->GetAttribute("AssetDef");
				if (assetDefAttrib.IsString())
				{
					Name assetDefType = assetDefAttrib.GetStringValue();
					EditorWindow::assetDefNameToEditorWindowClass[assetDefType].Add(classType);
				}
			}
		}

		void OnClassDeregistered(ClassType* classType)
		{
			if (classType == nullptr)
				return;

			if (classType->IsSubclassOf<EditorWindow>() && classType != EditorWindow::StaticType())
			{
				auto assetDefAttrib = classType->GetAttribute("AssetDef");
				if (assetDefAttrib.IsString())
				{
					Name assetDefType = assetDefAttrib.GetStringValue();
					EditorWindow::assetDefNameToEditorWindowClass[assetDefType].Remove(classType);
				}
			}
		}

		DelegateHandle onClassReg = 0;
		DelegateHandle onClassDereg = 0;
    };
}

#include "CrystalEditor.private.h"
#include "Resource.h"
CE_IMPLEMENT_MODULE(CrystalEditor, CE::CrystalEditorModule)

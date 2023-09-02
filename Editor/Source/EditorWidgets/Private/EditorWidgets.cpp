
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

			ObjectEditor::classTypeToEditorMap.Add({ TYPEID(Object), GetStaticClass<ObjectEditor>()});

			onClassRegistered = CoreObjectDelegates::onClassRegistered.AddDelegateInstance(MemberDelegate(&EditorWidgetsModule::OnClassRegistered, this));
			onClassDeregistered = CoreObjectDelegates::onClassDeregistered.AddDelegateInstance(MemberDelegate(&EditorWidgetsModule::OnClassDeregistered, this));
        }

        void ShutdownModule() override
        {
			ObjectEditor::classTypeToEditorMap.Clear();

			CoreObjectDelegates::onClassRegistered.RemoveDelegateInstance(onClassRegistered);
			CoreObjectDelegates::onClassDeregistered.RemoveDelegateInstance(onClassDeregistered);
			onClassRegistered = onClassDeregistered = 0;

			GetStyleManager()->RemoveResourceSearchModule(MODULE_NAME);
        }

        void RegisterTypes() override
        {
			
        }

		void OnClassRegistered(ClassType* classType)
		{
			if (classType == nullptr)
				return;
			
			if (classType->IsSubclassOf<ObjectEditor>() && classType->HasAttribute("TargetType"))
			{
				Name targetTypeName = classType->GetAttribute("TargetType").GetStringValue();
				if (targetTypeName.IsValid())
				{
					ObjectEditor::classTypeToEditorMap[targetTypeName].Add(classType);
				}
			}
			else if (classType->IsSubclassOf<FieldEditor>() && classType != GetStaticClass<FieldEditor>() && classType->HasAttribute("TargetType"))
			{
				Name targetTypeName = classType->GetAttribute("TargetType").GetStringValue();
				if (targetTypeName.IsValid())
				{
					FieldEditor::editorClassesByFieldType[targetTypeName].Add(classType);
				}
			}
		}

		void OnClassDeregistered(ClassType* classType)
		{
			if (classType == nullptr)
				return;

			if (classType->IsSubclassOf<ObjectEditor>() && classType->HasAttribute("TargetType"))
			{
				Name targetTypeName = classType->GetAttribute("TargetType").GetStringValue();
				if (targetTypeName.IsValid())
				{
					ObjectEditor::classTypeToEditorMap[targetTypeName].Remove(classType);
				}
			}
			else if (classType->IsSubclassOf<FieldEditor>() && classType != GetStaticClass<FieldEditor>() && classType->HasAttribute("TargetType"))
			{
				Name targetTypeName = classType->GetAttribute("TargetType").GetStringValue();
				if (targetTypeName.IsValid())
				{
					FieldEditor::editorClassesByFieldType[targetTypeName].Remove(classType);
				}
			}
		}

		DelegateHandle onClassRegistered = 0;
		DelegateHandle onClassDeregistered = 0;
    };
}

#include "EditorWidgets.private.h"
#include "Resource.h"

CE_IMPLEMENT_MODULE(EditorWidgets, CE::Editor::EditorWidgetsModule)

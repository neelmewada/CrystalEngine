#include "Drawers/StringFieldDrawer.h"

#include <QLabel>

namespace CE::Editor
{

    StringFieldDrawer::StringFieldDrawer() : FieldDrawer("StringFieldDrawer")
	{
		
	}

	StringFieldDrawer::~StringFieldDrawer()
	{

	}

	void StringFieldDrawer::OnValuesUpdated()
	{
        if (targetInstance == nullptr || stringField == nullptr)
            return;
        if (fieldType->GetDeclarationTypeId() != TYPEID(String))
            return;

        String value = fieldType->GetFieldValue<String>(targetInstance);
        stringField->SetValue(value);
	}

	void StringFieldDrawer::CreateGUI(QLayout* container)
	{
        if (!IsValid())
            return;

        if (stringField != nullptr)
        {
            delete stringField;
        }
        
        stringField = new Qt::StringField(container->parentWidget());
        
        stringField->Bind("OnTextInputChanged", this, "OnStringFieldInputChanged");
	}

    void StringFieldDrawer::OnStringFieldInputChanged(CE::String newText)
    {
        if (targetInstance == nullptr || stringField == nullptr)
            return;
        if (fieldType->GetDeclarationTypeId() != TYPEID(String))
            return;

        fieldType->SetFieldValue<String>(targetInstance, newText);
    }
}

CE_RTTI_CLASS_IMPL(CRYSTALEDITOR_API, CE::Editor, StringFieldDrawer)

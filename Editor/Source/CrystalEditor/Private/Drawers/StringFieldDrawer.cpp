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
        CE_LOG(Info, All, "New Text: {}", newText);
    }
}

CE_RTTI_CLASS_IMPL(CRYSTALEDITOR_API, CE::Editor, StringFieldDrawer)

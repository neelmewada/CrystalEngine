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

	}

}

CE_RTTI_CLASS_IMPL(CRYSTALEDITOR_API, CE::Editor, StringFieldDrawer)

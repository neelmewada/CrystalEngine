
#include "Drawers/VectorFieldDrawer.h"

#include <QLabel>

namespace CE::Editor
{

	VectorFieldDrawer::VectorFieldDrawer() : FieldDrawer("VectorFieldDrawer")
	{

	}

	VectorFieldDrawer::~VectorFieldDrawer()
	{

	}

	void VectorFieldDrawer::CreateGUI(QLayout* container)
	{
		if (!IsValid())
			return;

		QLabel* label = new QLabel(container->parentWidget());
		label->setText("Vector Field Drawer");
		container->addWidget(label);
	}


} // namespace CE::Editor

CE_RTTI_CLASS_IMPL(CRYSTALEDITOR_API, CE::Editor, VectorFieldDrawer)

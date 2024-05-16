#include "EditorSystem.h"

namespace CE::Editor
{

	PropertyEditorRow::PropertyEditorRow()
	{
		orientation = COrientation::Horizontal;
	}

	PropertyEditorRow::~PropertyEditorRow()
	{
		
	}

	void PropertyEditorRow::Construct()
	{
		Super::Construct();

		AddSplit(0.6f);

		left = GetContainer(0);
		left->SetName("PropertyEditorRowLeft");

		right = GetContainer(1);
		right->SetName("PropertyEditorRowRight");

	}


} // namespace CE::Editor

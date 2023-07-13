
#include "CrystalEditor.h"

namespace CE::Editor
{

	EditorPanel::EditorPanel()
	{

	}

	EditorPanel::~EditorPanel()
	{

	}

	void EditorPanel::Construct()
	{
		Super::Construct();


	}

	bool EditorPanel::IsSubWidgetAllowed(ClassType* subwidgetClass)
	{
		return subwidgetClass != nullptr && subwidgetClass->IsSubclassOf<EditorPanelView>();
	}

	void EditorPanel::OnDrawGUI()
	{
		Super::OnDrawGUI();
	}
}


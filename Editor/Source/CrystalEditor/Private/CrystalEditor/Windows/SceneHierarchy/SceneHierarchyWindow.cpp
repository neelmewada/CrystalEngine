#include "CrystalEditor.h"

namespace CE::Editor
{

	SceneHierarchyWindow::SceneHierarchyWindow()
	{
		dataModel = CreateDefaultSubobject<SceneDataModel>("DataModel");
	}

	SceneHierarchyWindow::~SceneHierarchyWindow()
	{
		
	}

	void SceneHierarchyWindow::SetScene(CE::Scene* scene)
	{
		dataModel->SetScene(scene);

		SetNeedsLayout();
		SetNeedsPaint();
	}

	void SceneHierarchyWindow::Construct()
	{
		Super::Construct();

		SetTitle("Hierarchy");

		hierarchyTreeView = CreateObject<CTreeView>(this, "HierarchyTreeView");
		hierarchyTreeView->SetModel(dataModel);

		hierarchyTreeView->SetColumnResizable(0, 0.6f);
		hierarchyTreeView->SetColumnResizable(1, 0.4f);
	}

} // namespace CE::Editor

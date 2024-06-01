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

		CItemSelectionModel* selectionModel = hierarchyTreeView->GetSelectionModel();

		selectionModel->onSelectionChanged += [this, selectionModel]
			{
				const CItemSelection& selection = selectionModel->GetSelection();
				onSceneSelectionChanged(const_cast<CItemSelection*>(&selection));

				if (selection.selectionRanges.NonEmpty())
				{
					Actor* actor = (Actor*)selection.selectionRanges[0].start.GetInternalData();
					if (actor)
					{
						onActorSelected(actor);
						return;
					}
				}

				onActorSelected(nullptr);
			};
	}

} // namespace CE::Editor

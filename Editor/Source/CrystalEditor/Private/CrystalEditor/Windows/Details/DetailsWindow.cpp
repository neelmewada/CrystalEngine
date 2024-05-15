#include "CrystalEditor.h"

namespace CE::Editor
{

    DetailsWindow::DetailsWindow()
    {
	    
    }

    DetailsWindow::~DetailsWindow()
    {
	    
    }

    void DetailsWindow::Construct()
    {
        Super::Construct();

        SetTitle("Details");

        LoadStyleSheet(PlatformDirectories::GetLaunchDir() / "Editor/Styles/DetailsWindowStyle.css");
      
        CSplitView* splitView = CreateObject<CSplitView>(this, "DetailsSplitter");
        splitView->SetOrientation(COrientation::Vertical);

        splitView->AddSplit(0.6f);

        CSplitViewContainer* topView = splitView->GetContainer(0);
        topView->SetName("SplitContainer");

        CSplitViewContainer* bottomView = splitView->GetContainer(1);
        bottomView->SetName("SplitContainer");
        bottomView->SetVerticalScrollAllowed(true);
        bottomView->AddBehavior<CScrollBehavior>();

        CWidget* header = CreateObject<CWidget>(topView, "HeaderRow");

        CLabel* title = CreateObject<CLabel>(header, "TitleLabel");
        title->SetText("SomeActorName");

        treeWidget = CreateObject<CTreeWidget>(topView, "ComponentTree");

        treeWidget->SetSelectionMode(CItemSelectionMode::SingleSelection);

        {
            CTreeWidgetItem* rootItem = CreateObject<CTreeWidgetItem>(treeWidget, "RootItem");
            rootItem->SetText("SomeActorName (Self)");
            rootItem->SetForceExpanded(true);

            {
                CTreeWidgetItem* sceneItem = CreateObject<CTreeWidgetItem>(rootItem, "SceneItem");
                sceneItem->SetText("SceneComponent");

                for (int i = 0; i < 2; i++)
                {
                    CTreeWidgetItem* meshComponentItem = CreateObject<CTreeWidgetItem>(sceneItem, "MeshItem");
                    meshComponentItem->SetText(String("StaticMeshComponent_") + i);

                    CTreeWidgetItem* meshComponentItem2 = CreateObject<CTreeWidgetItem>(meshComponentItem, "MeshItem");
                    meshComponentItem2->SetText("StaticMeshComponent_Child");
                }

                for (int i = 0; i < 3; ++i)
                {
	                CTreeWidgetItem* componentItem = CreateObject<CTreeWidgetItem>(rootItem, String("ActorComponent_") + i);
	                componentItem->SetText(String("ActorComponent_") + i);
	                componentItem->SetExpanded(false); // TODO: Perform manual update
	                componentItem->SetExpanded(true);
                }
            }
        }

        Bind(treeWidget, MEMBER_FUNCTION(CTreeWidget, OnSelectionChanged), [this](CTreeWidget*)
            {
                const auto& selectedItems = treeWidget->GetSelectedItems();

	            for (auto it = selectedItems.cbegin(); it != selectedItems.cend(); ++it)
	            {
                    CTreeWidgetItem* selected = *it;
                    break;
	            }
            });

        CCollapsibleSection* collapsibleSection = CreateObject<CCollapsibleSection>(bottomView, "CollapsibleSection");
        collapsibleSection->SetTitle("Expansible Section");

        for (int i = 0; i < 32; ++i)
        {
            CWidget* outer = collapsibleSection->GetContent();
            if (i >= 8)
                outer = bottomView;

            CButton* testButton = CreateObject<CButton>(outer, String("TestButton_") + i);
          
            testButton->SetText(String("Click Me ") + i);
        }
    }
    
} // namespace CE::Editor

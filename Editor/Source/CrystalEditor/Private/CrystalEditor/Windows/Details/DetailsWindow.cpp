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

        CLabel* title = CreateObject<CLabel>(this, "TitleLabel");
        title->SetText("SomeActorName");

        treeWidget = CreateObject<CTreeWidget>(this, "ComponentTree");
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
	                CTreeWidgetItem* componentItem = CreateObject<CTreeWidgetItem>(rootItem, "ActorComponent");
	                componentItem->SetText(String("ActorComponent_") + i);
	                componentItem->SetExpanded(false); // TODO: Perform manual update
	                componentItem->SetExpanded(true);
                }
            }
        }

        for (int i = 0; i < 64; ++i)
        {
            CButton* testButton = CreateObject<CButton>(this, String("TestButton_") + i);
            testButton->SetText(String("Click Me ") + i);
        }
    }
    
} // namespace CE::Editor

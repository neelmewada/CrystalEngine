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

            {
                CTreeWidgetItem* sceneItem = CreateObject<CTreeWidgetItem>(rootItem, "SceneItem");
                sceneItem->SetText("SceneComponent");

                {
                    CTreeWidgetItem* meshComponentItem = CreateObject<CTreeWidgetItem>(sceneItem, "MeshItem");
                    meshComponentItem->SetText("StaticMeshComponent");
                }

                CTreeWidgetItem* componentItem = CreateObject<CTreeWidgetItem>(rootItem, "ActorComponent");
                componentItem->SetText("ActorComponent");
                componentItem->SetExpanded(false); // TODO: Perform manual update
                componentItem->SetExpanded(true);
            }
        }
    }
    
} // namespace CE::Editor

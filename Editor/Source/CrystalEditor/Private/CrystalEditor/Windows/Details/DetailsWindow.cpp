#include "CrystalEditor.h"

namespace CE::Editor
{

    DetailsWindow::DetailsWindow()
    {
        receiveKeyEvents = true;
        receiveMouseEvents = true;
    }

    DetailsWindow::~DetailsWindow()
    {
	    
    }

    void DetailsWindow::SetupForActor(Actor* actor)
    {
        treeWidget->RemoveAllItems();

        if (!actor)
        {
            splitView->SetEnabled(false);
            noSelectionLabel->SetEnabled(true);

            UpdateLayoutIfNeeded();

	        return;
        }

        splitView->SetEnabled(true);
        noSelectionLabel->SetEnabled(false);

        CTreeWidgetItem* rootItem = CreateObject<CTreeWidgetItem>(treeWidget, "RootItem");
        rootItem->SetText( actor->GetName().GetString() + " (Self)");
        rootItem->SetForceExpanded(true);

        std::function<void(SceneComponent*, CTreeWidgetItem*)> sceneComponentVisitor = [&](SceneComponent* sceneComponent, CTreeWidgetItem* parentItem)
            {
                if (!sceneComponent)
                    return;

                CTreeWidgetItem* item = CreateObject<CTreeWidgetItem>(parentItem, sceneComponent->GetName().GetString());
                item->SetText(sceneComponent->GetName().GetString());

                for (int i = 0; i < sceneComponent->GetAttachedComponentCount(); ++i)
                {
                    SceneComponent* attachedComponent = sceneComponent->GetAttachedComponent(i);
                    sceneComponentVisitor(attachedComponent, item);
                }
            };

        sceneComponentVisitor(actor->GetRootComponent(), rootItem);
    }

    void DetailsWindow::Construct()
    {
        Super::Construct();

        SetTitle("Details");

        LoadStyleSheet(PlatformDirectories::GetLaunchDir() / "Editor/Styles/DetailsWindowStyle.css");

        noSelectionLabel = CreateObject<CLabel>(this, "NoSelectionLabel");
        noSelectionLabel->SetText("Select an object to view it's details");

        splitView = CreateObject<CSplitView>(this, "DetailsSplitter");
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

        splitView->SetEnabled(false);
        noSelectionLabel->SetEnabled(true);
    }

    void DetailsWindow::HandleEvent(CEvent* event)
    {

	    Super::HandleEvent(event);
    }

} // namespace CE::Editor

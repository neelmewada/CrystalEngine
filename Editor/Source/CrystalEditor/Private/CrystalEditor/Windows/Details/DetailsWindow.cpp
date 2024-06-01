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
        if (actor == selectedActor)
            return;

        treeWidget->RemoveAllItems();

        selectedActor = actor;

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
        rootItem->SetText(actor->GetName().GetString() + " (Self)");
        rootItem->SetInternalDataPtr(actor);
        rootItem->SetForceExpanded(true);

        std::function<void(SceneComponent*, CTreeWidgetItem*)> sceneComponentVisitor = [&](SceneComponent* sceneComponent, CTreeWidgetItem* parentItem)
            {
                if (!sceneComponent)
                    return;

                CTreeWidgetItem* item = CreateObject<CTreeWidgetItem>(parentItem, sceneComponent->GetName().GetString());
                item->SetText(sceneComponent->GetName().GetString());
                item->SetInternalDataPtr(sceneComponent);

                for (int i = 0; i < sceneComponent->GetAttachedComponentCount(); ++i)
                {
                    SceneComponent* attachedComponent = sceneComponent->GetAttachedComponent(i);
                    sceneComponentVisitor(attachedComponent, item);
                }
            };

        sceneComponentVisitor(actor->GetRootComponent(), rootItem);

        treeWidget->Select(rootItem);
    }

    void DetailsWindow::SetEditTarget(Object* target)
    {
        if (this->targetObject == target)
            return;

        this->targetObject = target;

        // Cleanup previous editor

        CSplitViewContainer* container = splitView->GetContainer(1);

        for (int i = container->GetSubWidgetCount() - 1; i >= 0; --i)
        {
            container->GetSubWidget(i)->Destroy();
        }

        if (objectEditor)
        {
            objectEditor->Destroy();
            objectEditor = nullptr;
        }

        if (targetObject)
        {
            objectEditor = ObjectEditor::Create(targetObject, this, "ObjectEditor");

            objectEditor->CreateGUI(container);
        }
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

        splitView->AddSplit(0.7f);

        CSplitViewContainer* topView = splitView->GetContainer(0);
        topView->SetName("SplitContainerTop");

        CSplitViewContainer* bottomView = splitView->GetContainer(1);
        bottomView->SetName("SplitContainerBottom");
        bottomView->SetVerticalScrollAllowed(true);
        bottomView->AddBehavior<CScrollBehavior>();

        CWidget* header = CreateObject<CWidget>(topView, "HeaderRow");

        titleLabel = CreateObject<CLabel>(header, "TitleLabel");
        titleLabel->SetText("Actor");

        treeWidget = CreateObject<CTreeWidget>(topView, "ComponentTree");
        treeWidget->SetSelectionMode(CItemSelectionMode::SingleSelection);

        treeWidget->onSelectionChanged += [this](CTreeWidget*)
            {
                const auto& selectedItems = treeWidget->GetSelectedItems();

                for (auto it = selectedItems.cbegin(); it != selectedItems.cend(); ++it)
                {
                    CTreeWidgetItem* selected = *it;
                    SetEditTarget((Object*)selected->GetInternalDataPtr());
                    return;
                }

                SetEditTarget(nullptr);
            };

        splitView->SetEnabled(false);
        noSelectionLabel->SetEnabled(true);
    }

    void DetailsWindow::HandleEvent(CEvent* event)
    {

	    Super::HandleEvent(event);
    }

} // namespace CE::Editor

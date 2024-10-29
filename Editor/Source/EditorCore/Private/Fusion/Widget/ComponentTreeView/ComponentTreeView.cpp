#include "EditorCore.h"

namespace CE::Editor
{

    ComponentTreeView::ComponentTreeView()
    {

    }

    void ComponentTreeView::Construct()
    {
        Super::Construct();

        Child(
            FNew(FScrollBox)
            .VerticalScroll(true)
            .HorizontalScroll(false)
            (
                FAssignNew(FVerticalStack, content)
                .HAlign(HAlign::Fill)
                .VAlign(VAlign::Top)
            )
        );
    }

    void ComponentTreeView::SetActor(Actor* actor)
    {
        content->DestroyAllChildren();

        for (ComponentTreeItem* item : items)
        {
            item->Destroy();
        }
        items.Clear();
        expandedItems.Clear();

        ComponentTreeItem* actorItem = CreateObject<ComponentTreeItem>(this, actor->GetName().GetString());
        actorItem->actor = actor;

        SceneComponent* sceneComponent = actor->GetRootComponent();

        std::function<void(SceneComponent* component)> visitor = [&](SceneComponent* component)
            {
                ComponentTreeViewRow* row = nullptr;

                FAssignNew(ComponentTreeViewRow, row)
				.Title(String::Format("{} ({})", component->GetName(), component->GetClass()->GetName().GetLastComponent()))
                    ;

                content->AddChild(row);

                for (int i = 0; i < component->GetAttachedComponentCount(); ++i)
                {
                    visitor(component->GetAttachedComponent(i));
                }
            };

        if (sceneComponent != nullptr)
        {
            visitor(sceneComponent);
        }
    }
}


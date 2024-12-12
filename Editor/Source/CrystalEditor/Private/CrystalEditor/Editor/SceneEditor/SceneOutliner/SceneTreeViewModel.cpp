#include "CrystalEditor.h"

namespace CE
{

    SceneTreeViewModel::SceneTreeViewModel()
    {

    }

    SceneTreeViewModel::~SceneTreeViewModel()
    {

    }

    FModelIndex SceneTreeViewModel::GetIndex(u32 row, u32 column, const FModelIndex& parent)
    {
        if (!scene)
            return {};

        if (!parent.IsValid())
        {
            if (row >= scene->GetRootActorCount())
                return {};

            return CreateIndex(row, column, scene->GetRootActor(row));
        }

        Actor* parentActor = (Actor*)parent.GetDataPtr();
        if (parentActor == nullptr || row >= parentActor->GetChildCount())
            return {};

        return CreateIndex(row, column, parentActor->GetChild(row));

    }

    FModelIndex SceneTreeViewModel::FindIndex(Actor* actor)
    {
        if (!actor || !actor->GetScene())
            return {};

        if (scene != actor->GetScene())
            return {};

        if (actor->GetParentActor() == nullptr)
        {
            int index = scene->GetIndexOfActor(actor);
            if (index < 0)
                return {};

            return GetIndex(index, 0, {});
        }
        
        FModelIndex parentIndex = FindIndex(actor->GetParentActor());
        if (!parentIndex.IsValid())
            return {};

        int index = actor->GetParentActor()->GetIndexOfActor(actor);
        if (index < 0)
            return {};

        return GetIndex(index, 0, parentIndex);
    }

    u32 SceneTreeViewModel::GetRowCount(const FModelIndex& parent)
    {
        if (!scene)
            return {};

        if (!parent.IsValid())
        {
            return scene->GetRootActorCount();
        }

        Actor* parentActor = (Actor*)parent.GetDataPtr();
        if (parentActor == nullptr)
            return {};

        return parentActor->GetChildCount();
    }

    u32 SceneTreeViewModel::GetColumnCount(const FModelIndex& parent)
    {
        return 2; // Name, Type
    }

    void SceneTreeViewModel::SetData(u32 row, FWidget& rowWidget, const FModelIndex& parent)
    {
        if (!scene)
            return;
        FTreeViewRow* rowCast = Object::CastTo<FTreeViewRow>(&rowWidget);
        if (rowCast == nullptr)
            return;

        FTreeViewRow& treeRow = *rowCast;

        FModelIndex index = GetIndex(row, 0, parent);
        if (!index.IsValid() || index.GetDataPtr() == nullptr)
            return;

        Actor* actor = (Actor*)index.GetDataPtr();
        String actorType = actor->GetType()->GetName().GetLastComponent();

        treeRow.GetCell(0)->Text(actor->GetName().GetString());
        treeRow.GetCell(1)->Text(actorType);
    }

    void SceneTreeViewModel::SetScene(CE::Scene* scene)
    {
        if (this->scene == scene)
            return;

        this->scene = scene;
        onModelUpdated(this);
    }

} // namespace CE


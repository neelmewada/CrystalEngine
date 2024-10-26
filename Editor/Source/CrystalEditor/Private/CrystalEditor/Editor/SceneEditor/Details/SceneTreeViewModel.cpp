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

            return CreateIndex(row, column, scene->GetRootActor(0));
        }

        Actor* parentActor = (Actor*)parent.GetDataPtr();
        if (parentActor == nullptr || row >= parentActor->GetChildCount())
            return {};

        return CreateIndex(row, column, parentActor->GetChild(row));

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

    void SceneTreeViewModel::SetData(u32 row, FTreeViewRow& rowWidget, const FModelIndex& parent)
    {

    }


    void SceneTreeViewModel::SetScene(CE::Scene* scene)
    {
        this->scene = scene;
        onModelUpdated(this);
    }

} // namespace CE


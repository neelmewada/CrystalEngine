#include "CrystalEditor.h"

namespace CE::Editor
{

    SceneDataModel::SceneDataModel()
    {
	    
    }

    SceneDataModel::~SceneDataModel()
    {
	    
    }

    u32 SceneDataModel::GetRowCount(const CModelIndex& parent)
    {
        if (scene == nullptr)
            return 0;

        return scene->GetRootActorCount();
    }

    u32 SceneDataModel::GetColumnCount(const CModelIndex& parent)
    {
        return 2;
    }

    CModelIndex SceneDataModel::GetParent(const CModelIndex& index)
    {
        Actor* actor = (Actor*)index.GetInternalData();
        if (!actor)
			return CModelIndex();

        Actor* parentActor = actor->GetParentActor();
        if (parentActor == nullptr)
        {
            return CModelIndex();
        }

        Actor* parentsParent = parentActor->GetParentActor();

        if (parentsParent == nullptr)
        {
	        for (int i = 0; i < scene->GetRootActorCount(); ++i)
	        {
		        if (scene->GetRootActor(i) == parentActor)
		        {
                    return CreateIndex(i, 0, parentActor);
		        }
	        }

            return CModelIndex(); // Should never happen: If parent actor not found in the scene
        }

        for (int i = 0; i < parentsParent->GetChildCount(); ++i)
        {
	        if (parentsParent->GetChild(i) == parentActor)
	        {
                return CreateIndex(i, 0, parentActor);
	        }
        }

        return CModelIndex(); // Should never happen: If parent actor not found in parent's parent actor
    }

    CModelIndex SceneDataModel::GetIndex(u32 row, u32 column, const CModelIndex& parent)
    {
        if (!parent.IsValid())
        {
            Actor* actor = scene->GetRootActor(row);
            return CreateIndex(row, column, actor);
        }

        Actor* actor = (Actor*)parent.GetInternalData();
        if (!actor)
			return CModelIndex();

        if (row >= actor->GetChildCount())
            return CModelIndex();
        
        return CreateIndex(row, column, actor->GetChild(row));
    }

    Variant SceneDataModel::GetData(const CModelIndex& index, CItemDataUsage usage)
    {
        if (!index.IsValid())
			return {};

    	Actor* actor = (Actor*)index.GetInternalData();
        if (!actor)
            return  {};

        if (usage == CItemDataUsage::Display)
        {
            if (index.GetColumn() == 0)
                return actor->GetName().GetString();
            if (index.GetColumn() == 1)
                return actor->GetClass()->GetName().GetLastComponent();
        }
        else if (usage == CItemDataUsage::Decoration && index.GetColumn() == 0)
        {
            if (actor->GetChildCount() == 0)
            {
                return "/Editor/Assets/Icons/File";
            }
            else
            {
                return "/Editor/Assets/Icons/Folder";
            }
        }

        return {};
    }

    Variant SceneDataModel::GetHeaderData(int position, COrientation orientation, CItemDataUsage usage)
    {
        if (usage == CItemDataUsage::Display && orientation == COrientation::Horizontal)
        {
            if (position == 0)
                return "Name";
            else if (position == 1)
                return "Type";
        }

        return Variant();
    }

    void SceneDataModel::SetScene(CE::Scene* scene)
    {
        this->scene = scene;

        emit OnModelDataUpdated();
    }

} // namespace CE::Editor

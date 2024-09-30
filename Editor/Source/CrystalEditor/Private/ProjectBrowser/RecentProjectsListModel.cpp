#include "CrystalEditor.h"

namespace CE::Editor
{
    RecentProjectItem::RecentProjectItem()
    {
	    
    }

    RecentProjectsListModel::RecentProjectsListModel()
    {

    }

    RecentProjectsListModel::~RecentProjectsListModel()
    {
        
    }

    void RecentProjectsListModel::ModelReset()
    {
        ProjectManager* projectManager = ProjectManager::Get();

        //const Array<String>& recentProjectPaths = projectManager->GetRecentProjectsList();

        // TODO: Remove test data
        recentProjectPaths.Clear();
        recentProjectPaths = {
			"C:/Projects/MyProj_1",
            "C:/Projects/MyProj_2",
            "C:/Projects/MyProj_3",
            "C:/Projects/MyProj_4",
            "C:/Projects/MyProj_5",
            "C:/Projects/MyProj_6",
            "C:/Projects/MyProj_7",
            "C:/Projects/MyProj_8",
            "C:/Projects/MyProj_9",
            "C:/Projects/MyProj_10",
            "C:/Projects/MyProj_11",
            "C:/Projects/MyProj_12",
        };

        Array<FListItem*> items = GetItemList();
        int maxCount = Math::Max(items.GetSize(), recentProjectPaths.GetSize());
        items.Reserve(maxCount);

        for (int i = 0; i < maxCount; ++i)
        {
            if (i < recentProjectPaths.GetSize())
            {
                RecentProjectItem* item = nullptr;
	            if (i < items.GetSize())
	            {
                    item = static_cast<RecentProjectItem*>(items[i]);
                    item->title = recentProjectPaths[i].GetString();
                    item->description = "This is the project description!";
	            }
                else
                {
                    item = CreateObject<RecentProjectItem>(this, "RecentItem");
                    item->title = recentProjectPaths[i].GetString();
                    item->description = "This is the project description!";
                    items.Add(item);
                }
            }
            else if (i < items.GetSize())
            {
                RecentProjectItem* item = static_cast<RecentProjectItem*>(items[i]);
                items.Remove(item);
                item->Destroy();
            }
        }

        SetItemList(items);
    }

} // namespace CE::Editor


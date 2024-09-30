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

        const Array<String>& recentProjectList = projectManager->GetRecentProjectsList();

        recentProjectPaths.Clear();

        for (const String& recentProject : recentProjectList)
        {
            recentProjectPaths.Add(recentProject);
        }

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


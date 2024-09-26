#include "CrystalEditor.h"

namespace CE::Editor
{
    RecentProjectItem::RecentProjectItem()
    {
	    
    }

    RecentProjectsModel::RecentProjectsModel()
    {

    }

    RecentProjectsModel::~RecentProjectsModel()
    {
        
    }

    void RecentProjectsModel::ModelReset()
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
        };

        Array<FListItem*> items = GetItemList();
        items.Reserve(Math::Max(items.GetSize(), recentProjectPaths.GetSize()));

        for (int i = 0; i < Math::Max(items.GetSize(), recentProjectPaths.GetSize()); ++i)
        {
	        if (i < items.GetSize())
	        {
                
	        }
            else
            {
	            
            }
        }
    }

} // namespace CE::Editor


#include "CrystalEditor.h"

namespace CE
{

    NewProjectListModel::NewProjectListModel()
    {

    }

    NewProjectListModel::~NewProjectListModel()
    {
        
    }

    void NewProjectListModel::ModelReset()
    {
        projectTemplates.Clear();
        projectTemplates = {
			{ "Empty Project", "Create a blank project" }
        };

        Array<FListItem*> items = GetItemList();
        int maxCount = Math::Max(items.GetSize(), projectTemplates.GetSize());
        items.Reserve(maxCount);

        for (int i = 0; i < maxCount; ++i)
        {
            if (i < projectTemplates.GetSize())
            {
                FListItem* item = nullptr;
                if (i < items.GetSize())
                {
                    item = static_cast<RecentProjectItem*>(items[i]);
                    item->title = projectTemplates[i].name.GetString();
                    item->description = projectTemplates[i].description;
                }
                else
                {
                    item = CreateObject<RecentProjectItem>(this, "NewProjectItem");
                    item->title = projectTemplates[i].name.GetString();
                    item->description = projectTemplates[i].description;
                    items.Add(item);
                }
            }
            else if (i < items.GetSize())
            {
                FListItem* item = items[i];
                items.Remove(item);
                item->BeginDestroy();
            }
        }

        SetItemList(items);
    }

} // namespace CE


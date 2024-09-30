#pragma once

namespace CE::Editor
{
    CLASS()
    class CRYSTALEDITOR_API RecentProjectItem : public FListItem
    {
        CE_CLASS(RecentProjectItem, FListItem)
    public:

        RecentProjectItem();

    };

    CLASS()
    class CRYSTALEDITOR_API RecentProjectsListModel : public FListViewModel
    {
        CE_CLASS(RecentProjectsListModel, FListViewModel)
    public:

        RecentProjectsListModel();

        virtual ~RecentProjectsListModel();

        // - Public API -

        void ModelReset();

    private:

        Array<IO::Path> recentProjectPaths{};

    public:



    };
    
} // namespace CE::Editor

#include "RecentProjectsListModel.rtti.h"

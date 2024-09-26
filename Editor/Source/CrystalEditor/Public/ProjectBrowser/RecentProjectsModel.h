#pragma once

namespace CE::Editor
{
    CLASS()
    class CRYSTALEDITOR_API RecentProjectItem : public FListItem
    {
        CE_CLASS(RecentProjectItem, FListItem)
    public:

        RecentProjectItem();

        FIELD()
        String description = "";

    };

    CLASS()
    class CRYSTALEDITOR_API RecentProjectsModel : public FListViewModel
    {
        CE_CLASS(RecentProjectsModel, FListViewModel)
    public:

        RecentProjectsModel();

        virtual ~RecentProjectsModel();

        // - Public API -

        void ModelReset();

    private:

        Array<IO::Path> recentProjectPaths{};

    public:



    };
    
} // namespace CE::Editor

#include "RecentProjectsModel.rtti.h"

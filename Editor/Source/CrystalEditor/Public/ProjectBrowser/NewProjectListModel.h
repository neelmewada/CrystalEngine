#pragma once

namespace CE
{
    CLASS()
    class CRYSTALEDITOR_API NewProjectListModel : public FListViewModel
    {
        CE_CLASS(NewProjectListModel, FListViewModel)
    public:

        virtual ~NewProjectListModel();

        // - Public API -

        void ModelReset();

    protected:

        NewProjectListModel();

        struct ProjectTemplateInfo
        {
            Name name;
            String description;
        };

        Array<ProjectTemplateInfo> projectTemplates{};

    };
    
} // namespace CE

#include "NewProjectListModel.rtti.h"

#pragma once

#include "ProjectSettings.h"

namespace CE
{
    
    class ProjectManager
    {
    private:
        ProjectManager() = default;
        ~ProjectManager() = default;

    public:

        static ProjectManager& Get()
        {
            static ProjectManager instance;
            return instance;
        }

        void GetCurrentProject();

    private:

        ProjectSettings project;
    };

} // namespace CE

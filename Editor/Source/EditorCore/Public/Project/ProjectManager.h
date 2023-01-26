#pragma once

#include "Project/ProjectSettings.h"

#include "IO/Path.h"

namespace CE::Editor
{

    /// Serves as an API for any Editor application to access a project & it's data, etc
    class EDITORCORE_API ProjectManager
    {
    private:
        ProjectManager();
        ~ProjectManager();

    public:
        
        static ProjectManager& Get()
        {
            static ProjectManager instance;
            return instance;
        }

        inline bool IsLoaded() const { return isLoaded; }

        void LoadProject(IO::Path projectPath);
        void UnloadProject();

    private:
        bool isLoaded = false;
        IO::Path loadedProjectPath{};
    };
    
} // namespace CE::Editor

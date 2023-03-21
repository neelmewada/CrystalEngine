#pragma once

#include "CoreMinimal.h"

#include "EditorProjectSettings.h"

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

        CE_INLINE bool IsProjectLoaded() const { return isLoaded; }
        CE_INLINE IO::Path GetLoadedProjectPath() const { return loadedProjectPath; }
        CE_INLINE IO::Path GetProjectBaseDirectory() const { return loadedProjectPath.GetParentPath(); }

        void CreateProject(IO::Path projectDirectory, String projectName);
        void LoadProject(IO::Path projectPath);
        void UnloadProject();

        ProjectSettings& GetCurrentProject();

    private:

        IO::Path loadedProjectPath{};
        bool isLoaded = false;
    };
    
} // namespace CE::Editor

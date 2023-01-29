#pragma once

#include "CoreTypes.h"

namespace CE
{
#if PAL_TRAIT_BUILD_EDITOR
    namespace Editor
    {
        class ProjectManager;
    }
#endif

    struct CORE_API ProjectSettings
    {
        CE_STRUCT(ProjectSettings)

    private:
        ProjectSettings();

        void LoadProjectSettings();

    public:

        static ProjectSettings& Get();

        CE_INLINE UUID GetProjectUuid() const { return projectUuid; }
        CE_INLINE String GetProjectName() const { return projectName; }
        CE_INLINE String GetProjectVersion() const { return projectVersion; }
        CE_INLINE String GetEngineVersion() const { return engineVersion; }

    private:

        static ProjectSettings currentProject;

        bool loaded = false;

        UUID projectUuid{};

        String projectName{};
        String projectVersion{};

        String engineVersion{};

#if PAL_TRAIT_BUILD_EDITOR
        friend class CE::Editor::ProjectManager;
#endif
    };
    
} // namespace CE

CE_RTTI_STRUCT(CORE_API, CE, ProjectSettings,
    CE_SUPER(),
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(projectUuid)
        CE_FIELD(projectName)
        CE_FIELD(projectVersion)
        CE_FIELD(engineVersion)
    )
)
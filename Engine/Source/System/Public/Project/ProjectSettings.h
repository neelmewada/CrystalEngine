#pragma once

#include "CoreMinimal.h"

namespace CE
{
#if PAL_TRAIT_BUILD_EDITOR
    namespace Editor
    {
        class ProjectManager;
    }
#endif

    struct SYSTEM_API ProjectSettings
    {
        CE_STRUCT(ProjectSettings)

    private:
        ProjectSettings();

        void LoadProjectSettings();

    public:

        static ProjectSettings& Get();

        String projectName{};
        String projectVersion{};

        String engineVersion = "0.1.0";

#if PAL_TRAIT_BUILD_EDITOR
        friend class CE::Editor::ProjectManager;
#endif
    };
    
} // namespace CE

CE_RTTI_STRUCT(SYSTEM_API, CE, ProjectSettings,
    CE_SUPER(),
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(projectName)
        CE_FIELD(projectVersion)
        CE_FIELD(engineVersion)
    )
)

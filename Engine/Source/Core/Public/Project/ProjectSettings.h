#pragma once

#include "CoreTypes.h"

namespace CE
{
#if PAL_TRAIT_BUILD_EDITOR
    namespace Editor
    {
        class ProjectManager;
        class EditorProjectSettings;
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
        CE_INLINE const String& GetProjectName() const { return projectName; }
        CE_INLINE const String& GetProjectVersionString() const { return projectVersion; }
        CE_INLINE const String& GetEngineVersion() const { return engineVersion; }

    private:

        static ProjectSettings currentProject;

        bool loaded = false;

        UUID projectUuid{};

        String projectName{};
        String projectVersion{};

        String engineVersion{};

        // Editor Project Settings
#if PAL_TRAIT_BUILD_EDITOR
        CE::Editor::EditorProjectSettings* editorProjectSettings = nullptr;

        friend class CE::Editor::ProjectManager;
        friend class CE::Editor::EditorProjectSettings;
#endif

        friend class AssetDatabase;
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

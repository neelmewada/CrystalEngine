#pragma once

#include "CoreMinimal.h"

namespace CE::Editor
{
    struct EDITORSYSTEM_API ProjectHistory
    {
        CE_STRUCT(ProjectHistory)

    public:

        UUID projectUuid;
        String projectPath;
        String projectName;

    };
    
    struct EDITORSYSTEM_API EditorPrefs : public IO::IFileWatchListener
    {
        CE_STRUCT(EditorPrefs)

    private:
        EditorPrefs() = default;
        virtual ~EditorPrefs();

    public:
        static void InvalidateCache() { cacheValid = false; }
        static void Reload();
        static void Save();

        static EditorPrefs& Get();

        void OnProjectOpened();

    private:
        virtual void HandleFileAction(IO::WatchID watchId, IO::Path directory, String fileName, IO::FileAction fileAction, String oldFileName) override;
        
        CE::Array<ProjectHistory> recentProjects;
        
        static IO::FileWatcher* watcher;
        static bool cacheValid;
        static EditorPrefs instance;
    };

} // namespace CE::Editor

CE_RTTI_STRUCT(EDITORSYSTEM_API, CE::Editor, ProjectHistory,
    CE_SUPER(),
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(projectUuid)
        CE_FIELD(projectPath)
        CE_FIELD(projectName)
    )
)

CE_RTTI_STRUCT(EDITORSYSTEM_API, CE::Editor, EditorPrefs,
    CE_SUPER(),
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(recentProjects)
    )
)


#include "Data/EditorPrefs.h"

#include "EditorCore.h"

namespace CE::Editor
{
    bool EditorPrefs::cacheValid = false;
    IO::FileWatcher* EditorPrefs::watcher = nullptr;
    
    EditorPrefs EditorPrefs::instance{};

    EditorPrefs::~EditorPrefs()
    {
        delete watcher;
        watcher = nullptr;
    }

    void EditorPrefs::Reload()
    {
        auto editorPrefsDir = PlatformDirectories::GetAppDataDir();
        auto editorPrefsPath = editorPrefsDir / "EditorPrefs.dat";

        if (!editorPrefsDir.Exists())
        {
            IO::Path::CreateDirectories(editorPrefsDir);
        }
        
        IO::FileStream file{};

        if (editorPrefsPath.Exists())
        {
            file = IO::FileStream(editorPrefsPath, IO::OpenMode::ModeRead);

            SerializedObject so{ Self::Type(), &instance };

            so.Deserialize(file);
        }
        else
        {
            file = IO::FileStream(editorPrefsPath, IO::OpenMode::ModeWrite); // ModeWrite will create the file if it doesn't exist

            SerializedObject so{ Self::Type(), &instance };

            so.Serialize(file);
        }

        cacheValid = true;

        if (instance.watcher == nullptr)
        {
            instance.watcher = new IO::FileWatcher();
            instance.watcher->AddWatcher(editorPrefsDir, &instance, false);
            instance.watcher->Watch();
        }
    }

    void EditorPrefs::Save()
    {
        auto editorPrefsDir = PlatformDirectories::GetAppDataDir();
        auto editorPrefsPath = editorPrefsDir / "EditorPrefs.dat";

        auto file = IO::FileStream(editorPrefsPath, IO::OpenMode::ModeWrite); // ModeWrite will create the file if it doesn't exist

        SerializedObject so{ Self::Type(), &instance };

        so.Serialize(file);

        // Invalidate cache
        cacheValid = false;
    }

    EditorPrefs& EditorPrefs::Get()
    {
        if (!cacheValid)
        {
            Reload();
        }

        return instance;
    }

    void EditorPrefs::OnProjectChanged()
    {
        ProjectSettings& project = ProjectSettings::Get();

        if (!recentProjects.Exists([project](const ProjectHistory& h) -> bool
            {
                return project.GetProjectUuid() == h.projectUuid;
            }))
        {
            ProjectHistory item{};
            item.projectUuid = project.GetProjectUuid();
            item.projectName = project.GetProjectName();
            item.projectPath = ProjectManager::Get().GetLoadedProjectPath().GetString();

            recentProjects.Add(item);

            Save();
        }
    }

    IO::Path EditorPrefs::GetLatestProjectPath()
    {
        if (recentProjects.GetSize() == 0)
        {
            return IO::Path();
        }

        return recentProjects[recentProjects.GetSize() - 1].projectPath;
    }

    void EditorPrefs::HandleFileAction(IO::WatchID watchId, IO::Path directory, String fileName, IO::FileAction fileAction, String oldFileName)
    {
        if (fileName == "EditorPrefs.dat")
        {
            cacheValid = false;
        }
    }

} // namespace CE::Editor

CE_RTTI_STRUCT_IMPL(EDITORSYSTEM_API, CE::Editor, ProjectHistory)

CE_RTTI_STRUCT_IMPL(EDITORSYSTEM_API, CE::Editor, EditorPrefs)

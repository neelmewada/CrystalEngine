
#include "Data/EditorPrefs.h"

namespace CE::Editor
{
    
    EditorPrefs EditorPrefs::editorPrefs{};

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

            SerializedObject so{ Self::Type(), &editorPrefs };

            so.Deserialize(file);
        }
        else
        {
            file = IO::FileStream(editorPrefsPath, IO::OpenMode::ModeWrite | IO::OpenMode::ModeRead); // ModeWrite will create the file if it doesn't exist

            SerializedObject so{ Self::Type(), &editorPrefs };

            so.Serialize(file);
        }
    }

    EditorPrefs& EditorPrefs::Load()
    {
        Reload();

        return editorPrefs;
    }

} // namespace CE::Editor

CE_RTTI_STRUCT_IMPL(EDITORSYSTEM_API, CE::Editor, EditorPrefs)

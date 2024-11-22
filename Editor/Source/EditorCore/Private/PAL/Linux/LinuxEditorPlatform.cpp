
#include "EditorCore.h"

#include <limits.h>

namespace CE::Editor
{

    IO::Path LinuxEditorPlatform::ShowSelectDirectoryDialog(const IO::Path& defaultPath)
    {
        IO::Path result{};

        String cmd = "zenity --file-selection --directory";
        if (defaultPath.Exists())
        {
            cmd += String::Format("--filename \"{}\"", defaultPath);
        }

        char filename[PATH_MAX];
        FILE *f = popen(cmd.GetCString(), "r");
        fgets(filename, PATH_MAX, f);
        result = filename;
        
        return result;
    }

    IO::Path LinuxEditorPlatform::ShowFileSelectionDialog(const IO::Path& defaultPath, const Array<FileType>& inFileTypes)
    {
        IO::Path result{};

        String cmd = "zenity --file-selection";
        if (defaultPath.Exists())
        {
            cmd += String::Format("--filename \"{}\"", defaultPath);
        }

        char filename[PATH_MAX];
        FILE *f = popen(cmd.GetCString(), "r");
        fgets(filename, PATH_MAX, f);
        result = filename;

        return result;
    }

    IO::Path LinuxEditorPlatform::GetEditorExecutablePath()
    {
        return PlatformDirectories::GetLaunchDir() / GetEditorExecutableName();
    }
} // namespace CE::Editor


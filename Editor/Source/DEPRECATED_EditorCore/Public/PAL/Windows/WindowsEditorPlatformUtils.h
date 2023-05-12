#pragma once

#if PLATFORM_WINDOWS

#include <QString>

namespace CE::Editor
{
    class EDITORCORE_API WindowsEditorPlatformUtils
    {
    public:
        WindowsEditorPlatformUtils() = delete;

        static void RevealInFileExplorer(const QString& path);
    };

    typedef WindowsEditorPlatformUtils EditorPlatformUtils;
}

#endif
#pragma once

#if PLATFORM_MAC

#include <QString>

namespace CE::Editor
{
    class EDITORCORE_API MacEditorPlatformUtils
    {
    public:
        MacEditorPlatformUtils() = delete;

        static void RevealInFileExplorer(const QString& path);
    };

    typedef MacEditorPlatformUtils EditorPlatformUtils;
}

#endif
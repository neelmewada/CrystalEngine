
#if PLATFORM_WINDOWS

#include <PAL/Windows/WindowsEditorPlatformUtils.h>

#include "EditorCore.h"

#include <QDir>
#include <QProcess>

namespace CE::Editor
{

    void WindowsEditorPlatformUtils::RevealInFileExplorer(const QString& path)
    {
        QStringList args;
        args << "/select," << QDir::toNativeSeparators(path);
        QProcess::startDetached("explorer", args);
    }
}

#endif
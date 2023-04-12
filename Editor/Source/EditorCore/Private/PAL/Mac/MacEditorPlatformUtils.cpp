
#if PLATFORM_MAC

#include <PAL/Windows/MacEditorPlatformUtils.h>

#include "EditorCore.h"

#include <QDir>
#include <QProcess>

namespace CE::Editor
{

    void MacEditorPlatformUtils::RevealInFileExplorer(const QString& path)
    {
        QStringList args;
        args << "-e";
        args << "tell application \"Finder\"";
        args << "-e";
        args << "activate";
        args << "-e";
        args << "select POSIX file \""+path+"\"";
        args << "-e";
        args << "end tell";
        QProcess::startDetached("osascript", args);
    }
}

#endif
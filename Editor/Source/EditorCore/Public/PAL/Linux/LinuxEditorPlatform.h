#pragma once

namespace CE::Editor
{

    class EDITORCORE_API LinuxEditorPlatform : public EditorPlatformBase
    {
        CE_STATIC_CLASS(LinuxEditorPlatform)
    public:

        static IO::Path ShowSelectDirectoryDialog(const IO::Path& defaultPath);

        static IO::Path ShowFileSelectionDialog(const IO::Path& defaultPath, const Array<FileType>& fileTypes);

        static IO::Path GetEditorExecutablePath();
    };

    typedef LinuxEditorPlatform EditorPlatform;

} // namespace CE::Editor


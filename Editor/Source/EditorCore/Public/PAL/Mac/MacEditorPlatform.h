#pragma once

namespace CE::Editor
{

    class EDITORCORE_API MacEditorPlatform : public EditorPlatformBase
    {
        CE_STATIC_CLASS(MacEditorPlatform)
    public:

        static IO::Path GetEditorExecutablePath();
        
        static IO::Path ShowSelectDirectoryDialog(const IO::Path& defaultPath);

        static IO::Path ShowFileSelectionDialog(const IO::Path& defaultPath, const Array<FileType>& fileTypes);
    };

    typedef MacEditorPlatform EditorPlatform;
    
} // namespace CE::Editor

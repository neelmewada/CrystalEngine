#pragma once

namespace CE::Editor
{

    class EDITORCORE_API MacEditorPlatform : public EditorPlatformBase
    {
        CE_STATIC_CLASS(MacEditorPlatform)
    public:
        
        static IO::Path ShowSelectDirectoryDialog(const IO::Path& defaultPath);

    };

    typedef MacEditorPlatform EditorPlatform;
    
} // namespace CE::Editor

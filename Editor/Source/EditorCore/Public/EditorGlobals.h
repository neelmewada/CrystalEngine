#pragma once

namespace CE::Editor
{

    enum class EditorMode
    {
        Default,
        ProjectBrowser,
        AssetProcessor
    };
    ENUM_CLASS(EditorMode);

    extern EDITORCORE_API EditorMode gEditorMode;
    
} // namespace CE::Editor

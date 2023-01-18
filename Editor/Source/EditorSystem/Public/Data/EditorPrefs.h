#pragma once

#include "CoreMinimal.h"

namespace CE::Editor
{
    
    struct EDITORSYSTEM_API EditorPrefs
    {
        CE_STRUCT(EditorPrefs)

    private:
        EditorPrefs() = default;

    public:

        static EditorPrefs* Load();
        
        String latestProjectPath{};
        String latestProjectName{};

    private:
        static EditorPrefs editorPrefs;
    };

} // namespace CE::Editor

CE_RTTI_STRUCT(EDITORSYSTEM_API, CE::Editor, EditorPrefs,
    CE_SUPER(),
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(latestProjectPath)
        CE_FIELD(latestProjectName)
    )
)

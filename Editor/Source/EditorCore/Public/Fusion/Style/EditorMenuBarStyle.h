#pragma once

namespace CE
{
    CLASS()
    class EDITORCORE_API EditorMenuBarStyle : public Object
    {
        CE_CLASS(EditorMenuBarStyle, Object)
    public:

        virtual ~EditorMenuBarStyle();

    protected:

        EditorMenuBarStyle();

    };
    
} // namespace CE

#include "EditorMenuBarStyle.rtti.h"

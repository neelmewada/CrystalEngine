#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORCORE_API EditorMenuBar : public FMenuBar
    {
        CE_CLASS(EditorMenuBar, FMenuBar)
    public:

        // - Public API -

    protected:

        EditorMenuBar();

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "EditorMenuBar.rtti.h"

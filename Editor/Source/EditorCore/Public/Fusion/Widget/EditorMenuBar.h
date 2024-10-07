#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORCORE_API EditorMenuBar : public FWidget
    {
        CE_CLASS(EditorMenuBar, FWidget)
    public:

        // - Public API -

    protected:

        EditorMenuBar();

        void Construct() override;

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "EditorMenuBar.rtti.h"

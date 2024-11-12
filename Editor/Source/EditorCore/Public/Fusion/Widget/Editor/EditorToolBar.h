#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORCORE_API EditorToolBar : public FStyledWidget
    {
        CE_CLASS(EditorToolBar, FStyledWidget)
    public:

        // - Public API -

    protected:

        EditorToolBar();

        void Construct() override;

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "EditorToolBar.rtti.h"

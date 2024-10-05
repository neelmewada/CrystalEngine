#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORCORE_API EditorDockTab : public FWidget
    {
        CE_CLASS(EditorDockTab, FWidget)
    public:

        // - Public API -

    protected:

        EditorDockTab();

        void Construct() override;

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "EditorDockTab.rtti.h"

#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORCORE_API EditorMinorDockspace : public FStyledWidget
    {
        CE_CLASS(EditorMinorDockspace, FStyledWidget)
    public:

        // - Public API -

    protected:

        EditorMinorDockspace();

        void Construct() override;

        FVerticalStack* rootBox = nullptr;
        FStyledWidget* titleBar = nullptr;
        FHorizontalStack* tabWell = nullptr;
        FStyledWidget* content = nullptr;

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "EditorMinorDockspace.rtti.h"

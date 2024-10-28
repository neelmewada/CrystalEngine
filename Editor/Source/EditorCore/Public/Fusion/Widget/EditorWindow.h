#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORCORE_API EditorWindow : public FWindow
    {
        CE_CLASS(EditorWindow, FWindow)
    public:

        // - Public API -

    protected:

        EditorWindow();

        void Construct() override;

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "EditorWindow.rtti.h"

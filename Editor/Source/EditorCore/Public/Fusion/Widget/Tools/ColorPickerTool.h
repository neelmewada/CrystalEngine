#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORCORE_API ColorPickerTool : public FToolWindow
    {
        CE_CLASS(ColorPickerTool, FToolWindow)
    protected:

        ColorPickerTool();

        void Construct() override;

    public: // - Public API -


    protected: // - Internal -


    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "ColorPickerTool.rtti.h"

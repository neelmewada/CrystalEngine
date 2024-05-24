#pragma once

namespace CE::Editor
{

    CLASS()
    class EDITORSYSTEM_API ColorPickerTool final : public CToolWindow
    {
        CE_CLASS(ColorPickerTool, CToolWindow)
    public:

        ColorPickerTool();

        virtual ~ColorPickerTool();

        static ColorPickerTool* Open();

    private:

        void Construct() override;

    };
    
} // namespace CE::Editor

#include "ColorPickerTool.rtti.h"
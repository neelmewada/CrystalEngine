#include "EditorCore.h"

namespace CE::Editor
{

    ColorPickerTool::ColorPickerTool()
    {

    }

    void ColorPickerTool::Construct()
    {
        Super::Construct();

        (*this)
            .Title("Color Picker")
            .MinimizeEnabled(false)
            .MaximizeEnabled(false)
            .ContentPadding(Vec4(1, 1, 1, 1) * 20)
            //.Content()
        ;
    }
    
}


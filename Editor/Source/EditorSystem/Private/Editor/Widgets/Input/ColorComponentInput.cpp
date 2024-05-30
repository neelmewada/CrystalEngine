#include "EditorSystem.h"

namespace CE::Editor
{

    ColorComponentInput::ColorComponentInput()
    {

    }

    void ColorComponentInput::Construct()
    {
        Super::Construct();

        SetRange(0, 1);

    }
    
}


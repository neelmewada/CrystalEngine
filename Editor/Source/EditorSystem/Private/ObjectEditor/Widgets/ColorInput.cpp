#include "EditorSystem.h"

namespace CE::Editor
{

    ColorInput::ColorInput()
    {
	    
    }

    ColorInput::~ColorInput()
    {
	    
    }

    void ColorInput::Construct()
    {
        Super::Construct();


    }

    void ColorInput::OnPaint(CPaintEvent* paintEvent)
    {
	    Super::OnPaint(paintEvent);

        CPainter* painter = paintEvent->painter;

        Vec2 pos = GetComputedLayoutTopLeft();
        Vec2 size = GetComputedLayoutSize();

        
    }

} // namespace CE::Editor

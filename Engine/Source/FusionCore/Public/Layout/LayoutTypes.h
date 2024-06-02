#pragma once

namespace CE
{
    ENUM()
    enum FHorizontalAlignment
    {
        ECONST(DisplayName = "Left")
	    HAlign_Left,
        ECONST(DisplayName = "Center")
        HAlign_Center,
        ECONST(DisplayName = "Right")
        HAlign_Right
    };

    ENUM()
    enum FVerticalAlignment
    {
        ECONST(DisplayName = "Top")
        VAlign_Top,
        ECONST(DisplayName = "Middle")
        VAlign_Middle,
        ECONST(DisplayName = "Bottom")
        VAlign_Bottom
    };
    

} // namespace CE

#include "LayoutTypes.rtti.h"
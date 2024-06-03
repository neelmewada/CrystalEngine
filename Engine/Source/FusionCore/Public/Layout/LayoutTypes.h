#pragma once

namespace CE
{
    ENUM()
    enum class HAlign
    {
	    Left,
        Center,
        Right,
        Fill
    };

    ENUM()
    enum class VAlign
    {
        Top,
        Center,
        Bottom,
        Fill
    };
    

} // namespace CE

#include "LayoutTypes.rtti.h"
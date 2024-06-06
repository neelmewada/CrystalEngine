#pragma once

namespace CE
{
    ENUM()
    enum class HAlign
    {
        Fill,
	    Left,
        Center,
        Right
    };

    ENUM()
    enum class VAlign
    {
        Fill,
        Top,
        Center,
        Bottom
    };
    

} // namespace CE

#include "LayoutTypes.rtti.h"
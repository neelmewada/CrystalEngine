#include "FusionCore.h"

namespace CE
{

    FScrollBoxStyle::FScrollBoxStyle()
    {

    }

    FScrollBoxStyle::~FScrollBoxStyle()
    {
        
    }

    SubClass<FWidget> FScrollBoxStyle::GetWidgetClass() const
    {
        return FScrollBox::StaticType();
    }

    void FScrollBoxStyle::MakeStyle(FWidget& widget)
    {
        FScrollBox& scrollBox = widget.As<FScrollBox>();

        scrollBox
            .ScrollBarBackground(background)
            .ScrollBarBrush(color)
            .ScrollBarHoverBrush(hoverColor);
    }

} // namespace CE


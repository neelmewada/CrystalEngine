#include "Fusion.h"

namespace CE
{

    FListItemStyle::FListItemStyle()
    {

    }

    FListItemStyle::~FListItemStyle()
    {
        
    }

    SubClass<FWidget> FListItemStyle::GetWidgetClass() const
    {
        return FListItemWidget::StaticType();
    }

    void FListItemStyle::MakeStyle(FWidget& widget)
    {
        FListItemWidget& itemWidget = widget.As<FListItemWidget>();

        
    }

} // namespace CE


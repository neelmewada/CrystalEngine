#include "FusionCore.h"

namespace CE
{

    FStyledWidgetStyle::FStyledWidgetStyle()
    {

    }

    FStyledWidgetStyle::~FStyledWidgetStyle()
    {
        
    }

    SubClass<FWidget> FStyledWidgetStyle::GetWidgetClass() const
    {
        return FStyledWidget::StaticType();
    }

    void FStyledWidgetStyle::MakeStyle(FWidget& widget)
    {
        FStyledWidget& styledWidget = widget.As<FStyledWidget>();

        styledWidget
            .Background(background)
			.BackgroundShape(shape)
			.Border(borderColor, borderWidth)
            ;
    }

} // namespace CE


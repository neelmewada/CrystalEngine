#include "FusionCore.h"

namespace CE
{

    FStyle::FStyle()
    {

    }

    FStyle::~FStyle()
    {
        
    }

    SubClass<FWidget> FStyle::GetWidgetClass() const
    {
        return FWidget::StaticType();
    }

} // namespace CE


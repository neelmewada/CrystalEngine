#include "FusionCore.h"

namespace CE
{

    FMenuStyle::FMenuStyle()
    {

    }

    SubClass<FWidget> FMenuStyle::GetWidgetClass() const
    {
        return FMenuPopup::StaticType();
    }

    void FMenuStyle::MakeStyle(FWidget& widget)
    {
        FMenuPopup& menu = widget.As<FMenuPopup>();

        for (int i = 0; i < menu.GetMenuItemCount(); ++i)
        {
            FMenuItem& item = *menu.GetMenuItem(i);
            
        }
    }
    
} // namespace CE


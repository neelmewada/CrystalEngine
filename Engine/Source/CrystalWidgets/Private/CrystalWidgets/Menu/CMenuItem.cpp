#include "CrystalWidgets.h"

namespace CE::Widgets
{

    CMenuItem::CMenuItem()
    {
        icon = CreateDefaultSubobject<CWidget>("Icon");
        label = CreateDefaultSubobject<CLabel>("Label");

        label->SetText("Menu Item");
    }

    CMenuItem::~CMenuItem()
    {
	    
    }

    void CMenuItem::SetIcon(Name iconPath)
    {
        icon->SetBackgroundImage(iconPath);
    }

    void CMenuItem::SetText(const String& text)
    {
        label->SetText(text);
    }

} // namespace CE::Widgets

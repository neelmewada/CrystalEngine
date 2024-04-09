#include "CrystalWidgets.h"

namespace CE::Widgets
{

    CMenuItem::CMenuItem()
    {
        receiveMouseEvents = receiveKeyEvents = true;

        icon = CreateDefaultSubobject<CWidget>("Icon");
        label = CreateDefaultSubobject<CLabel>("Label");

        label->SetText(GetName().GetString());


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

    bool CMenuItem::IsSubWidgetAllowed(Class* subWidgetClass)
    {
        return subWidgetClass->IsSubclassOf<CWidget>();
    }

    void CMenuItem::OnSubobjectAttached(Object* object)
    {
        Super::OnSubobjectAttached(object);

        if (!object)
            return;

        if (object->IsOfType<CMenu>())
        {
            subMenu = static_cast<CMenu*>(object);
        }
    }

    void CMenuItem::OnSubobjectDetached(Object* object)
    {
	    Super::OnSubobjectDetached(object);

        if (!object)
            return;

        if (object->IsOfType<CMenu>() && subMenu == (CMenu*)object)
        {
            subMenu = nullptr;
        }
    }

} // namespace CE::Widgets

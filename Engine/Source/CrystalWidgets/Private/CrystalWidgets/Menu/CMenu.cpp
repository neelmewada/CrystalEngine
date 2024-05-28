#include "CrystalWidgets.h"

namespace CE::Widgets
{

    CMenu::CMenu()
    {
        canBeClosed = canBeMaximized = canBeMinimized = false;

        receiveMouseEvents = receiveKeyEvents = true;
    }

    CMenu::~CMenu()
    {
        
    }

    void CMenu::Show()
    {
        Super::Show();

        //isShown = true;
    }

    void CMenu::Show(Vec2i screenPosition, Vec2i size)
    {
        Super::Show(screenPosition, size);

        //isShown = true;
    }

    void CMenu::OnPlatformWindowSet()
    {
	    Super::OnPlatformWindowSet();
    }

    void CMenu::Hide()
    {
        for (auto menuItem : menuItems)
        {
            menuItem->SetStateFlags(CStateFlag::Default);

            menuItem->HideSubMenu();
        }

        Super::Hide();

        //isShown = false;
    }

    void CMenu::OnSubobjectAttached(Object* subobject)
    {
	    Super::OnSubobjectAttached(subobject);
        
        if (!subobject)
            return;

        if (subobject->IsOfType<CMenuItem>())
        {
            menuItems.Add(static_cast<CMenuItem*>(subobject));
        }
    }

    void CMenu::OnSubobjectDetached(Object* subobject)
    {
        Super::OnSubobjectDetached(subobject);

        if (!subobject)
            return;

        if (subobject->IsOfType<CMenuItem>())
        {
            menuItems.Remove(static_cast<CMenuItem*>(subobject));
        }
    }

    bool CMenu::WindowHitTest(PlatformWindow* window, Vec2 position)
    {
        return false;
    }

    void CMenu::HandleEvent(CEvent* event)
    {
        if (event->IsMouseEvent())
        {
            
        }

        Super::HandleEvent(event);
    }


} // namespace CE::Widgets

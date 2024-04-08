#include "CrystalWidgets.h"

namespace CE::Widgets
{

    CMenu::CMenu()
    {
        canBeClosed = canBeMaximized = canBeMinimized = false;
    }

    CMenu::~CMenu()
    {
	    
    }

    void CMenu::Show()
    {
        Super::Show();
    }

    void CMenu::Show(Vec2i screenPosition, Vec2i size)
    {
        Super::Show(screenPosition, size);
    }

    void CMenu::Hide()
    {
        Super::Hide();
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

} // namespace CE::Widgets

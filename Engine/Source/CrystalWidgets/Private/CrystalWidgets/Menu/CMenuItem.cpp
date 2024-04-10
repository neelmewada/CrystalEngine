#include "CrystalWidgets.h"

namespace CE::Widgets
{

    CMenuItem::CMenuItem()
    {
        receiveMouseEvents = receiveKeyEvents = true;

        icon = CreateDefaultSubobject<CWidget>("Icon");
        label = CreateDefaultSubobject<CLabel>("Label");

        label->SetText(GetName().GetString());

        spacer = CreateDefaultSubobject<CWidget>("Spacer");
        spacer->AddStyleClass("Spacer");

        forwardArrow = CreateDefaultSubobject<CWidget>("ForwardArrow");
        forwardArrow->SetBackgroundImage("/Engine/Assets/Icons/ForwardArrow");

        forwardArrow->SetVisible(false);
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
        return subWidgetClass->IsSubclassOf<CWidget>() && !subWidgetClass->IsSubclassOf<CMenu>();
    }

    void CMenuItem::HideSubMenu()
    {
        if (subMenu)
        {
            stateFlags &= ~CStateFlag::Active;

            subMenu->Hide();
        }
    }

    void CMenuItem::ShowSubMenu()
    {
        if (subMenu)
        {
            Rect rect = GetScreenSpaceRect();
            Vec2 pos = rect.min + Vec2(rect.GetSize().width, 0);
            subMenu->UpdateStyleIfNeeded();
            subMenu->UpdateLayoutIfNeeded();

            Vec2 menuSize = subMenu->GetComputedLayoutSize();

            stateFlags |= CStateFlag::Active;

            subMenu->Show(pos.ToVec2i(), menuSize.ToVec2i());
        }
    }

    void CMenuItem::OnSubobjectAttached(Object* object)
    {
        Super::OnSubobjectAttached(object);

        if (!object)
            return;

        if (object->IsOfType<CMenu>())
        {
            subMenu = static_cast<CMenu*>(object);
            forwardArrow->SetVisible(true);
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
            forwardArrow->SetVisible(false);
        }
    }

    void CMenuItem::HandleEvent(CEvent* event)
    {
        if (!event->isConsumed && event->IsMouseEvent())
        {
            CMouseEvent* mouseEvent = static_cast<CMouseEvent*>(event);

            if (event->type == CEventType::MousePress && mouseEvent->button == MouseButton::Left)
            {
                event->Consume(this);
            }
            else if (event->type == CEventType::MouseRelease && mouseEvent->button == MouseButton::Left)
            {
                emit OnMenuItemClicked();
                event->Consume(this);
            }
            else if (event->type == CEventType::MouseEnter && parent && parent->IsOfType<CMenu>())
            {
                CMenu* parentMenu = static_cast<CMenu*>(parent);

                for (int i = 0; i < parentMenu->GetMenuItemCount(); ++i)
                {
                    CMenuItem* menuItem = parentMenu->GetMenuItem(i);

                    if (menuItem != this)
                    {
                        menuItem->HideSubMenu();
                    }
                }

                ShowSubMenu();

                SetNeedsPaint();
            }
        }

        Super::HandleEvent(event);
    }


} // namespace CE::Widgets

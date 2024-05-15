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
        if (subMenu && subMenu->IsShown())
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
            subMenu->ownerItem = this;
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
            subMenu->ownerItem = nullptr;
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
                SetNeedsPaint();
                event->Consume(this);
            }
            else if (event->type == CEventType::MouseRelease && mouseEvent->button == MouseButton::Left && mouseEvent->isInside)
            {
                emit OnMenuItemClicked(this);
                event->Consume(this);
                
                if (subMenu == nullptr) // A leaf menu item
                {
                    auto parent = this->parent;
                    CMenu* topMostMenu = nullptr;

                    while (parent != nullptr)
                    {
	                    if (parent->IsOfType<CMenu>())
	                    {
                            CMenu* menu = static_cast<CMenu*>(parent);
                            topMostMenu = menu;

                            parent = topMostMenu->ownerItem;
	                    }
                        else
                        {
                            parent = parent->parent;
                        }
                    }

                    if (topMostMenu)
                    {
                        topMostMenu->Hide();
                    }
                }
            }
            else if (event->type == CEventType::MouseEnter && parent && parent->IsOfType<CMenu>() && SubWidgetExistsRecursive(event->sender))
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
                event->Consume(this);
            }

            SetNeedsPaint();
        }

        Super::HandleEvent(event);
    }


} // namespace CE::Widgets

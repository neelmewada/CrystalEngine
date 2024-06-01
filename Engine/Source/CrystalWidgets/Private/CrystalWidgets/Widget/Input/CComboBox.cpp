#include "CrystalWidgets.h"

namespace CE::Widgets
{
    CComboBoxItem::CComboBoxItem()
    {
        receiveMouseEvents = true;
    }

    void CComboBoxItem::Construct()
    {
        Super::Construct();

        label = CreateObject<CLabel>(this, "ItemLabel");
    }

    void CComboBoxItem::HandleEvent(CEvent* event)
    {
        if (event->IsMouseEvent())
        {
            CMouseEvent* mouseEvent = static_cast<CMouseEvent*>(event);

            if (event->type == CEventType::MousePress && mouseEvent->button == MouseButton::Left)
            {
                comboBox->SelectItem(this);
                comboBox->ClosePopup();
            }
        }

	    Super::HandleEvent(event);
    }


    CComboBox::CComboBox()
    {
        receiveMouseEvents = true;
    }

    CComboBox::~CComboBox()
    {
        
    }

    bool CComboBox::IsSubWidgetAllowed(Class* subWidgetClass)
    {
        if (subWidgetClass->IsSubclassOf<CComboBoxItem>())
            return false;
        if (subWidgetClass->IsSubclassOf<CPopup>())
            return false;

	    return Super::IsSubWidgetAllowed(subWidgetClass);
    }

    void CComboBox::AddItem(CComboBoxItem* item)
    {
        if (items.Exists(item))
            return;

        item->comboBox = this;
        items.Add(item);
        comboPopup->AddSubWidget(item);
    }

    void CComboBox::RemoveItem(CComboBoxItem* item)
    {
        if (!items.Exists(item))
            return;

        item->comboBox = nullptr;
        items.Remove(item);
        comboPopup->RemoveSubWidget(item);
    }

    void CComboBox::OpenPopup()
    {
        for (int i = 0; i < items.GetSize(); ++i)
        {
            if (i == selectedItemIndex)
                items[i]->stateFlags |= CStateFlag::Active;
            else
                items[i]->stateFlags &= ~CStateFlag::Active;
        }

        comboPopup->SetNeedsStyle();
        comboPopup->SetNeedsLayout();

        comboPopup->UpdateStyleIfNeeded();
        comboPopup->UpdateLayoutIfNeeded();

        SetNeedsLayout();
        comboPopup->SetNeedsLayout();

        Rect rect = GetScreenSpaceRect();
        Vec2 comboBoxSize = rect.GetSize();

        Vec2 size = comboPopup->GetComputedLayoutSize();
        size.width = rect.GetSize().width;

        Vec2 pos = rect.min + Vec2(0, (int)rect.GetSize().height);
        rect = Rect::FromSize(pos, size);

        CWindow* rootWindow = GetRootWindow();

    	if (rootWindow)
    	{
            Rect windowRect = rootWindow->GetScreenSpaceRect();

    		if (rect.max.y > windowRect.max.y)
    		{
                rect = rect.Translate(Vec2(0, -size.y - comboBoxSize.height));
    		}
    	}

        comboPopup->Show(rect.min.ToVec2i(), rect.GetSize().ToVec2i());
    }

    void CComboBox::ClosePopup()
    {
        comboPopup->Hide();
    }

    void CComboBox::SelectItem(int index)
    {
        if (index < 0 || index >= items.GetSize())
            return;

        selectedItemIndex = index;
        displayLabel->SetText(items[index]->GetText());

        onSelectionChanged(index);
    }

    void CComboBox::SelectItem(CComboBoxItem* item)
    {
        if (!item)
            return;

        int index = items.IndexOf(item);
        SelectItem(index);
    }

    void CComboBox::Construct()
    {
        Super::Construct();

        comboPopup = CreateObject<CPopup>(this, "ComboPopup");
        comboPopup->SetEnabled(false);
        comboPopup->hideWhenUnfocused = true;

        displayLabel = CreateObject<CLabel>(this, "DisplayLabel");
        displayLabel->SetText("[Select]");

        CWidget* spacer = CreateObject<CWidget>(this, "Spacer");
        spacer->AddStyleClass("Spacer");

        downardArrow = CreateObject<CImage>(this, "DownwardArrow");
        downardArrow->SetBackgroundImage("/Engine/Assets/Icons/DownwardArrow");
    }

    void CComboBox::HandleEvent(CEvent* event)
    {
        if (event->IsMouseEvent())
        {
            CMouseEvent* mouseEvent = static_cast<CMouseEvent*>(event);

            if (mouseEvent->type == CEventType::MousePress && mouseEvent->button == MouseButton::Left)
            {
                if (comboPopup->IsShown())
                    ClosePopup();
                else
                    OpenPopup();
            }
        }

	    Super::HandleEvent(event);
    }

    void CComboBox::OnPaint(CPaintEvent* paintEvent)
    {
	    Super::OnPaint(paintEvent);

        
    }

    void CComboBox::OnSubobjectAttached(Object* subobject)
    {
        Super::OnSubobjectAttached(subobject);

        if (subobject->IsOfType<CComboBoxItem>())
        {
            CComboBoxItem* item = (CComboBoxItem*)subobject;
            AddItem(item);
        }
    }

    void CComboBox::OnSubobjectDetached(Object* subobject)
    {
	    Super::OnSubobjectDetached(subobject);

    }

}


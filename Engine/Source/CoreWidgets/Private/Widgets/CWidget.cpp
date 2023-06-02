
#include "CoreWidgets.h"

namespace CE::Widgets
{

	CWidget::CWidget()
	{
		
	}

	void CWidget::OnSubobjectAttached(Object* subobject)
	{
		if (subobject == nullptr)
			return;
		if (subobject->GetClass()->IsSubclassOf<CWidget>())
		{
			CWidget* subWidget = (CWidget*)subobject;
			attachedWidgets.Add(subWidget);
			OnWidgetAttached(subWidget);
			subWidget->OnAttachedTo(this);
		}
	}

	void CWidget::OnSubobjectDetached(Object* subobject)
	{
		if (subobject == nullptr)
			return;
		if (subobject->GetClass()->IsSubclassOf<CWidget>())
		{
			CWidget* subWidget = (CWidget*)subobject;
			attachedWidgets.Remove(subWidget);
			OnWidgetDetached(subWidget);
			subWidget->OnDetachedFrom(this);
		}
	}
    
	CWidget::~CWidget()
	{
        
	}

	void CWidget::RenderGUI()
	{        
		OnDrawGUI();

		isFocused = GUI::IsItemFocused();
		isHovered = GUI::IsItemHovered();
		isLeftClicked = GUI::IsItemClicked(GUI::MouseButton::Left);
		isRightClicked = GUI::IsItemClicked(GUI::MouseButton::Right);
		isMiddleClicked = GUI::IsItemClicked(GUI::MouseButton::Middle);
    }

    void CWidget::SetWidgetFlags(WidgetFlags flags)
    {
        this->widgetFlags = flags;
    }

    WidgetFlags CWidget::GetWidgetFlags() const
    {
        return widgetFlags;
    }
	

} // namespace CE::Widgets



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

    void CWidget::BeginStyle()
    {
        
    }

    void CWidget::EndStyle()
    {
        if (pushedVars > 0)
            GUI::PopStyleVar(pushedVars);
        if (pushedColors > 0)
            GUI::PopStyleColor(pushedColors);
    }

	void CWidget::RenderGUI()
	{
        BeginStyle();
		OnDrawGUI();

		isFocused = GUI::IsItemFocused();
		isHovered = GUI::IsItemHovered();
		isLeftClicked = GUI::IsItemClicked(GUI::MouseButton::Left);
		isRightClicked = GUI::IsItemClicked(GUI::MouseButton::Right);
		isMiddleClicked = GUI::IsItemClicked(GUI::MouseButton::Middle);
        
        EndStyle();
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



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

    void CWidget::BuildWidget()
    {
        if (isBuilt)
            return;
        
        Build();
        isBuilt = true;
    }

    void CWidget::BeginStyle()
    {
        for (const auto& [variable, value] : style.styles)
        {
            auto styleVars = CStyle::GetStyleVar(variable);
            for (GUI::StyleVar styleVar : styleVars)
            {
                if (styleVar != GUI::StyleVar_COUNT)
                {
                    if (GUI::IsStyleVarOfVectorType(styleVar))
                        GUI::PushStyleVar(styleVar, (Vec2)value.vector);
                    else
                        GUI::PushStyleVar(styleVar, value.single);
                    pushedVars++;
                }
            }
            auto styleColors = CStyle::GetStyleColorVar(variable);
            for (GUI::StyleCol styleCol : styleColors)
            {
                if (styleCol != GUI::StyleCol_COUNT)
                {
                    GUI::PushStyleColor(styleCol, value.color);
                    pushedColors++;
                }
            }
        }
    }

    void CWidget::EndStyle()
    {
        if (pushedVars > 0)
            GUI::PopStyleVar(pushedVars);
        if (pushedColors > 0)
            GUI::PopStyleColor(pushedColors);
        
        pushedVars = pushedColors = 0;
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


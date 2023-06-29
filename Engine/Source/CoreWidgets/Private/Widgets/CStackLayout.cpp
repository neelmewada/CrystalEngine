#include "CoreWidgets.h"

namespace CE::Widgets
{

    CStackLayout::CStackLayout()
    {
        
    }
    
    CStackLayout::~CStackLayout()
    {
        
    }
    
    void CStackLayout::SetDirection(CStackDirection direction)
    {
        AddStyleProperty(CStylePropertyType::FlexDirection,
                         direction == CStackDirection::Horizontal ? CFlexDirection::Row : CFlexDirection::Column);
    }

    CStackDirection CStackLayout::GetDirection()
{
        auto& ref = style.GetProperty(CStylePropertyType::FlexDirection);
        if (ref.IsValid() && ref.IsEnum())
        {
            auto dir = (CFlexDirection)ref.enumValue.x;
            if (dir == CFlexDirection::Column)
                return CStackDirection::Vertical;
            else if (dir == CFlexDirection::ColumnReverse)
                return CStackDirection::Vertical;
            else if (dir == CFlexDirection::Row)
                return CStackDirection::Horizontal;
            else if (dir == CFlexDirection::RowReverse)
                return CStackDirection::Horizontal;
        }
        return CStackDirection::Vertical;
    }
    
    
    void CStackLayout::OnDrawGUI()
    {
        for (CWidget* subWidget : attachedWidgets)
        {
            subWidget->RenderGUI();
        }
    }

    void CStackLayout::HandleEvent(CEvent* event)
    {
        
        Super::HandleEvent(event);
    }


} // namespace CE::Widgets

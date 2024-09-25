#include "FusionCore.h"

namespace CE
{

    FTabViewStyle::FTabViewStyle()
    {
        tabItemShape = FRoundedRectangle(5, 5, 0, 0);

        tabItemPadding = Vec4(1, 1, 1, 1) * 5;
    }

    FTabViewStyle::~FTabViewStyle()
    {
        
    }

    SubClass<FWidget> FTabViewStyle::GetWidgetClass() const
    {
        return FTabView::StaticType();
    }

    void FTabViewStyle::MakeStyle(FWidget& widget)
    {
        FTabView& tabView = widget.As<FTabView>();

        for (int i = 0; i < tabView.GetTabItemCount(); ++i)
        {
            FTabItem& tabItem = *tabView.GetTabItem(i);

            FBrush bg = tabItemBackground;

            if (tabItem.IsActive())
                bg = tabItemActiveBackground;
            else if (tabItem.IsHovered())
                bg = tabItemHoverBackground;

            tabItem
                .Background(bg)
                .BackgroundShape(tabItemShape)
                .Padding(tabItemPadding)
        	;
        }

        FStyledWidget* container = tabView.GetContainer();

        if (container)
        {
            
        }
    }

} // namespace CE

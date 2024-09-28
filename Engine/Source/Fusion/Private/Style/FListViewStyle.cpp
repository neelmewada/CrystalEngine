#include "Fusion.h"

namespace CE
{

    FListViewStyle::FListViewStyle()
    {

    }

    FListViewStyle::~FListViewStyle()
    {
        
    }

    SubClass<FWidget> FListViewStyle::GetWidgetClass() const
    {
        return FListView::StaticType();
    }

    void FListViewStyle::MakeStyle(FWidget& widget)
    {
	    Super::MakeStyle(widget);

        FListView& listView = widget.As<FListView>();

        for (FListItemWidget* itemWidgetPtr : listView.itemWidgets)
        {
            FListItemWidget& itemWidget = *itemWidgetPtr;

            FBrush bg = background;
            Color border = borderColor;

            if (itemWidget.IsSelected())
            {
                bg = selectedItemBackground;
                border = selectedItemBorderColor;
            }
            else if (itemWidget.IsHovered())
            {
                bg = hoveredItemBackground;
                border = hoveredItemBorderColor;
            }

            itemWidget
                .Background(bg)
                .Border(border, borderWidth)
                ;
        }
    }

} // namespace CE


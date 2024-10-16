#include "Fusion.h"

namespace CE
{

    FTreeViewStyle::FTreeViewStyle()
    {

    }

    FTreeViewStyle::~FTreeViewStyle()
    {
        
    }

    SubClass<FWidget> FTreeViewStyle::GetWidgetClass() const
    {
        return FTreeView::StaticType();
    }

    void FTreeViewStyle::MakeStyle(FWidget& widget)
    {
        FTreeView& treeView = widget.As<FTreeView>();

        treeView
			.Background(background)
            ;

        if (treeView.GetHeaderContainer() != nullptr)
        {
	        FStyledWidget& header = *treeView.GetHeaderContainer();

        	header
				.Background(headerBackground)
			;
        }

        if (treeView.header)
        {
            FTreeViewHeader& header = *treeView.header;

            for (FStyledWidget* separator : header.separators)
            {
                (*separator)
					.Background(headerSeparator)
					.Width(separatorWidth)
                    ;
            }
        }
    }

} // namespace CE


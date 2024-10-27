#include "EditorCore.h"

namespace CE
{

    EditorDockspaceStyle::EditorDockspaceStyle()
    {

    }

    EditorDockspaceStyle::~EditorDockspaceStyle()
    {

    }

    SubClass<FWidget> EditorDockspaceStyle::GetWidgetClass() const
    {
        return EditorDockspace::StaticType();
    }

    void EditorDockspaceStyle::MakeStyle(FWidget& widget)
    {
        EditorDockspace& dockspace = widget.As<EditorDockspace>();

        dockspace
			.Background(background)
			.BorderColor(borderColor)
			.BorderWidth(borderWidth)
			.TitleBarBackground(titleBarBackground)
    	;

        for (int i = 0; i < dockspace.tabItems.GetSize(); ++i)
        {
            EditorDockTabItem& tab = *dockspace.tabItems[i];

            FBrush bg = inactiveTabBackground;
            Color tabBorder = inactiveTabBorder;

            if (tab.isActive)
            {
	            bg = activeTabBackground;
                tabBorder = activeTabBorder;
            }
            else if (tab.isHovered)
            {
                bg = hoveredTabBackground;
                tabBorder = hoveredTabBorder;
            }

            //bg = activeTabBackground;

            tab
				.Background(bg)
				.Border(tabBorder, tabBorderWidth)
				.CornerRadius(tabCornerRadius)
        	;
        }
    }
    
} // namespace CE


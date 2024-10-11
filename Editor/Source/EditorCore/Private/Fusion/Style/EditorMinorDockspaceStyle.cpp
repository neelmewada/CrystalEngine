#include "EditorCore.h"

namespace CE
{

    EditorMinorDockspaceStyle::EditorMinorDockspaceStyle()
    {

    }

    EditorMinorDockspaceStyle::~EditorMinorDockspaceStyle()
    {

    }

    SubClass<FWidget> EditorMinorDockspaceStyle::GetWidgetClass() const
    {
        return EditorMinorDockspace::StaticType();
    }

    void EditorMinorDockspaceStyle::MakeStyle(FWidget& widget)
    {
        EditorMinorDockspace& dockspace = widget.As<EditorMinorDockspace>();

        dockspace
            .TitleBarBackground(titleBarBackground)
			.Background(background)
			.BorderColor(borderColor)
			.BorderWidth(borderWidth)
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

            bg = activeTabBackground;

            tab
				.Background(bg)
				.Border(tabBorder, tabBorderWidth)
				.CornerRadius(tabCornerRadius)
        	;
        }
    }
    
} // namespace CE


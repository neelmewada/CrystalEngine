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

        dockspace.GetProjectLabelParent()->Background(projectLabelBackground);

        for (int i = 0; i < dockspace.GetTabItemCount(); ++i)
        {
            EditorDockTabItem& tab = *dockspace.GetTabItem(i);

            FBrush bg = inactiveTabBackground;
            Color tabBorder = inactiveTabBorder;

            if (tab.IsActive())
            {
	            bg = activeTabBackground;
                tabBorder = activeTabBorder;
            }
            else if (tab.IsHovered())
            {
                bg = hoveredTabBackground;
                tabBorder = hoveredTabBorder;
            }

            tab
				.Background(bg)
				.Border(tabBorder, tabBorderWidth)
				.CornerRadius(tabCornerRadius)
        	;
        }
    }
    
} // namespace CE


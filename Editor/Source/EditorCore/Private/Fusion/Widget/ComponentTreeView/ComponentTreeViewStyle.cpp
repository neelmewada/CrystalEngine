#include "EditorCore.h"

namespace CE::Editor
{

    ComponentTreeViewStyle::ComponentTreeViewStyle()
    {
        background = Color::RGBA(26, 26, 26);
    }

    ComponentTreeViewStyle::~ComponentTreeViewStyle()
    {
        
    }

    SubClass<FWidget> ComponentTreeViewStyle::GetWidgetClass() const
    {
        return ComponentTreeView::Type();
    }

    void ComponentTreeViewStyle::MakeStyle(FWidget& widget)
    {
        Super::MakeStyle(widget);

        ComponentTreeView& treeView = widget.As<ComponentTreeView>();
        int rowCount = 0;

        for (ComponentTreeViewRow* row : treeView.rows)
        {
	        if (!row->Enabled())
                continue;

            FBrush rowBg = (rowCount % 2 == 0) ? rowBackground : rowAlternateBackground;

            if (row->IsSelected())
                rowBg = rowSelectedBackground;
            else if (row->IsHovered())
                rowBg = rowHoverBackground;

            row->Background(rowBg);

            rowCount++;
        }
    }

} // namespace CE


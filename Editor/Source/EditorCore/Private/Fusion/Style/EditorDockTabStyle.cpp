#include "EditorCore.h"

namespace CE
{

    EditorDockTabStyle::EditorDockTabStyle()
    {

    }

    EditorDockTabStyle::~EditorDockTabStyle()
    {

    }

    SubClass<FWidget> EditorDockTabStyle::GetWidgetClass() const
    {
        return EditorDockTab::StaticType();
    }

    void EditorDockTabStyle::MakeStyle(FWidget& widget)
    {
        EditorDockTab& dockTab = widget.As<EditorDockTab>();

        dockTab
			.ContentBackground(background)
			.ContentBorderColor(borderColor)
			.ContentBorderWidth(borderWidth)
            ;
    }
    
} // namespace CE


#include "EditorCore.h"

namespace CE
{

    EditorMenuBarStyle::EditorMenuBarStyle()
    {
        itemFontSize = 10;
    }

    SubClass<FWidget> EditorMenuBarStyle::GetWidgetClass() const
    {
        return EditorMenuBar::StaticType();
    }

    void EditorMenuBarStyle::MakeStyle(FWidget& widget)
    {
	    Super::MakeStyle(widget);

        EditorMenuBar& menuBar = widget.As<EditorMenuBar>();

        
    }

    EditorMenuBarStyle::~EditorMenuBarStyle()
    {
        
    }
    
} // namespace CE


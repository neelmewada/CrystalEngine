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

        String::IsAlphabet('a');
    }
    
} // namespace CE


#include "EditorWidgets.h"

#include "OpenSans.h"


namespace CE::Editor
{

	void EditorStyles::InitDefaultStyle()
	{
		using namespace CE::Widgets;

		GetStyleManager()->LoadGlobalStyleSheet("/EditorWidgets/Resources/Style.css");
        
        GUI::Style& style = GUI::GetStyle();
        style.colors[GUI::StyleCol_Button] = Color::FromRGBA32(64, 64, 64).ToVec4();
        style.colors[GUI::StyleCol_ButtonHovered] = Color::FromRGBA32(90, 90, 90).ToVec4();
        style.colors[GUI::StyleCol_ButtonActive] = Color::FromRGBA32(50, 50, 50).ToVec4();
	}

	void EditorStyles::GetDefaultFont(unsigned char** outFont, unsigned int* outLength)
	{
		*outFont = OpenSans_VariableFont_ttf;
		*outLength = OpenSans_VariableFont_len;
	}

} // namespace CE::Editor

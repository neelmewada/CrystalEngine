#include "EditorWidgets.h"

#include "OpenSans.h"

static const CE::String globalStyleSheet = R"(
CWindow {
	foreground: white;
	background: rgb(36, 36, 36);
	padding: 5 25 5 5;
	flex-direction: column;
	align-items: flex-start;
	row-gap: 5px;
}

CLabel {
	text-align: middle-center;
}

CButton {
	padding: 10px 1px;
	border-radius: 2px;
	border-width: 1.5px;
	border-color: rgb(25, 25, 25);
	background: rgb(64, 64, 64);
}
CButton:hovered {
	background: rgb(90, 90, 90);
}
CButton:pressed {
	background: rgba(50, 50, 50);
}

CButton::alternate {
	padding: 10px 1px;
	border-radius: 2px;
	border-width: 1.5px;
	border-color: rgb(25, 25, 25);
	background: rgb(0, 112, 224);
}

CButton::alternate:hovered {
	background: rgb(14, 134, 255);
}

CButton::alternate:pressed {
	background: rgb(9, 93, 178);
}

CTextInput {
	background: rgb(15, 15, 15);
	border-width: 1;
	border-radius: 1 1 1 1;
	border-color: rgb(42, 42, 42);
	padding: 3 3 3 3;
}
CTextInput::hint {
	foreground: rgba(255, 255, 255, 120);
}

CTabWidget {
	width: 100%;
	height: 100%;
}

CTabWidget::tab {
	padding: 10 10 10 10;
	background: rgba(22, 22, 22, 210);
}

CTabWidget::tab:hovered {
	background: rgba(90, 90, 90, 210);
}

CTabWidget::tab:active {
	background: rgba(60, 60, 60, 255);
}

CTabWidget::tab:unfocused {
	background: rgba(22, 22, 22);
}

CTabContainerWidget {
	padding: 3 0 3 0;
}

)";

namespace CE::Editor
{

	void EditorStyles::InitDefaultStyle()
	{
		using namespace CE::Widgets;

		GetStyleManager()->SetGlobalStyleSheet(globalStyleSheet);
	}

	void EditorStyles::GetDefaultFont(unsigned char** outFont, unsigned int* outLength)
	{
		*outFont = OpenSans_VariableFont_ttf;
		*outLength = OpenSans_VariableFont_len;
	}

} // namespace CE::Editor

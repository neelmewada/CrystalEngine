#include "EditorWidgets.h"

#include "OpenSans.h"

static const CE::String globalStyleSheet = R"(
CWindow {
	foreground: white;
	background: rgb(36, 36, 36);
	padding: 8 25 8 10;
	flex-direction: column;
	align-items: flex-start;
	row-gap: 5px;
}

CWindow::title-bar {
	background: rgb(20, 20, 20);
}

CWindow::title-bar:active {
	background: rgb(20, 20, 20);
}

CWindow::tab {
	background: rgb(28, 28, 28);
}

CWindow::tab:hovered {
	background: rgb(32, 32, 32);
}

CWindow::tab:active {
	background: rgb(36, 36, 36);
}

CWindow.DockSpace {
	background: rgb(21, 21, 21);
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
	background: rgb(6, 66, 126);
}

CButton.Transparent {
	padding: 10px 2px;
	border-width: 0px;
	background: rgba(0, 0, 0, 0);
}

CButton.Transparent:hovered {
	background: rgba(70, 70, 70, 180);
}

CButton.Transparent:pressed {
	background: rgba(50, 50, 50, 180);
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
		
CCollapsibleSection::header {
	font-size: 18px;
	background: rgb(47, 47, 47);
	border-width: 1px;
	border-color: rgb(30, 30, 30);
	padding: 3px 5px;
	border-radius: 0px;
}

CSeparator:vertical {
	border-width: 0px;
	background: rgb(26, 26, 26);
	width: 100%;
	height: 2px;
}

CSeparator:horizontal {
	border-width: 0px;
	background: rgb(26, 26, 26);
	width: 2px;
	height: 100%;
}

CMenu {
	background: rgb(50, 50, 50);
	border-width: 1px;
	border-color: rgb(26, 26, 26);
	min-width: 120px;
	min-height: 20px;
	max-width: 350px;
	padding: 0px 10px;
}

.MenuItem {
	border-width: 0px;
	border-color: rgba(0, 0, 0, 0);
	text-align: middle-left;
	padding: 10px 0 10px 0;
}

CMenuItem:hovered, CMenuItem:pressed, CMenuItem:active {
	background: rgb(0, 112, 224);
}

CMenuItemHeader {
	foreground: rgb(108, 108, 108);
}

CMenuBar {
	background: rgb(21, 21, 21);
	border-width: 0px;
	min-height: 20px;
	max-height: 60px;
	flex-direction: row;
	justify-content: flex-start;
	align-items: stretch;
}

.MenuBarItem {
	text-align: middle-center;
	padding: 10px 0px;
}

.MenuBarItem:hovered {
	background: rgb(0, 112, 224);
}

)";

namespace CE::Editor
{

	void EditorStyles::InitDefaultStyle()
	{
		using namespace CE::Widgets;

		GetStyleManager()->SetGlobalStyleSheet(globalStyleSheet);
        
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

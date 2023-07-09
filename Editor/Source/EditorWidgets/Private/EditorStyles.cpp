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

		/*gStyleManager->globalStyle.AddProperty(CStylePropertyType::Background, Color::FromRGBA32(36, 36, 36), CStateFlag::Default, CSubControl::Window);

		gStyleManager->globalStyle.AddProperty(CStylePropertyType::Background, Color::FromRGBA32(47, 47, 47), CStateFlag::Default, CSubControl::Header);
		gStyleManager->globalStyle.AddProperty(CStylePropertyType::Background, Color::FromRGBA32(60, 60, 60), CStateFlag::Hovered, CSubControl::Header);
		gStyleManager->globalStyle.AddProperty(CStylePropertyType::Background, Color::FromRGBA32(70, 70, 70), CStateFlag::Pressed, CSubControl::Header);

		gStyleManager->globalStyle.AddProperty(CStylePropertyType::Background, Color::FromRGBA32(15, 15, 15), CStateFlag::Default, CSubControl::Frame);
		gStyleManager->globalStyle.AddProperty(CStylePropertyType::Background, Color::FromRGBA32(28, 28, 28), CStateFlag::Hovered, CSubControl::Frame);
		gStyleManager->globalStyle.AddProperty(CStylePropertyType::Background, Color::FromRGBA32(40, 40, 40), CStateFlag::Pressed, CSubControl::Frame);

		gStyleManager->globalStyle.AddProperty(CStylePropertyType::BorderWidth, 1.5f, CStateFlag::Default, CSubControl::None);
		gStyleManager->globalStyle.AddProperty(CStylePropertyType::BorderRadius, 1.0f, CStateFlag::Default, CSubControl::Frame);
		gStyleManager->globalStyle.AddProperty(CStylePropertyType::BorderColor, Color::FromRGBA32(45, 45, 45), CStateFlag::Default, CSubControl::None);

		gStyleManager->globalStyle.AddProperty(CStylePropertyType::Background, Color::White(), CStateFlag::Default, CSubControl::CheckMark);
		gStyleManager->globalStyle.AddProperty(CStylePropertyType::Background, Color(0.08f, 0.08f, 0.09f, 0.83f), CStateFlag::Default, CSubControl::Tab);
		gStyleManager->globalStyle.AddProperty(CStylePropertyType::Background, Color(0.33f, 0.34f, 0.36f, 0.83f), CStateFlag::Hovered, CSubControl::Tab);
		gStyleManager->globalStyle.AddProperty(CStylePropertyType::Background, Color(0.23f, 0.23f, 0.24f, 1.00f), CStateFlag::Active, CSubControl::Tab);
		gStyleManager->globalStyle.AddProperty(CStylePropertyType::Background, Color(0.08f, 0.08f, 0.09f, 1.00f), CStateFlag::Unfocused, CSubControl::Tab);

		gStyleManager->globalStyle.AddProperty(CStylePropertyType::Background, Color(0.08f, 0.08f, 0.09f, 1.00f), CStateFlag::Default, CSubControl::TitleBar);
		gStyleManager->globalStyle.AddProperty(CStylePropertyType::Background, Color(0.08f, 0.08f, 0.09f, 1.00f), CStateFlag::Active, CSubControl::TitleBar);
		gStyleManager->globalStyle.AddProperty(CStylePropertyType::Background, Color(0.00f, 0.00f, 0.00f, 0.51f), CStateFlag::Collapsed, CSubControl::TitleBar);
		gStyleManager->globalStyle.AddProperty(CStylePropertyType::ForegroundColor, Color::White());

		gStyleManager->globalStyle.AddProperty(CStylePropertyType::Padding, Vec4(10, 5, 10, 5), CStateFlag::Default, CSubControl::Tab);

		auto& buttonSelector = gStyleManager->AddStyleGroup("CButton");
		buttonSelector.AddProperty(CStylePropertyType::Background, Color(0.25f, 0.25f, 0.25f, 1.00f), CStateFlag::Default);
		buttonSelector.AddProperty(CStylePropertyType::Background, Color(0.38f, 0.38f, 0.38f, 1.00f), CStateFlag::Hovered);
		buttonSelector.AddProperty(CStylePropertyType::Background, Color(0.67f, 0.67f, 0.67f, 0.39f), CStateFlag::Pressed);

		auto& collapsibleSelector = gStyleManager->GetStyleGroup("CCollapsibleSection");
		collapsibleSelector.AddProperty(CStylePropertyType::BorderColor, Color::FromRGBA32(30, 30, 30), CStateFlag::Default, CSubControl::Header);
		collapsibleSelector.AddProperty(CStylePropertyType::Padding, Vec4(4, 4, 4, 4), CStateFlag::Default, CSubControl::Header);
		collapsibleSelector.AddProperty(CStylePropertyType::BorderRadius, Vec4(), CStateFlag::Default, CSubControl::Header);

		auto& textInputSelector = gStyleManager->GetStyleGroup(".TextField");
		textInputSelector.AddProperty(CStylePropertyType::Padding, Vec4(5, 6, 5, 6));
		textInputSelector.AddProperty(CStylePropertyType::Width, CStyleValue(100, true));
		textInputSelector.AddProperty(CStylePropertyType::BorderRadius, Vec4(1, 1, 1, 1) * 1.5f);

		auto& selectableWidgetSelector = gStyleManager->GetStyleGroup("CSelectableWidget");
		selectableWidgetSelector.AddProperty(CStylePropertyType::BorderColor, Color::FromRGBA32(0, 145, 197, 100), CStateFlag::Hovered);
		selectableWidgetSelector.AddProperty(CStylePropertyType::BorderColor, Color::FromRGBA32(0, 145, 197, 180), CStateFlag::Pressed);
		selectableWidgetSelector.AddProperty(CStylePropertyType::BorderColor, Color::FromRGBA32(0, 145, 197, 70), CStateFlag::Active);

		selectableWidgetSelector.AddProperty(CStylePropertyType::Background, Color::FromRGBA32(0, 145, 197, 80), CStateFlag::Hovered);
		selectableWidgetSelector.AddProperty(CStylePropertyType::Background, Color::FromRGBA32(0, 145, 197, 150), CStateFlag::Pressed);
		selectableWidgetSelector.AddProperty(CStylePropertyType::Background, Color::FromRGBA32(0, 145, 197, 50), CStateFlag::Active);
		*/
	}

	void EditorStyles::GetDefaultFont(unsigned char** outFont, unsigned int* outLength)
	{
		*outFont = OpenSans_VariableFont_ttf;
		*outLength = OpenSans_VariableFont_len;
	}

} // namespace CE::Editor

#include "EditorWidgets.h"

#include "OpenSans.h"

namespace CE::Editor
{

	void EditorStyles::InitDefaultStyle()
	{
		using namespace CE::Widgets;

		gStyleManager->globalStyle.AddProperty(CStylePropertyType::Background, Color::FromRGBA32(36, 36, 36), CStateFlag::Default, CSubControl::Window);

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

		gStyleManager->globalStyle.AddProperty(CStylePropertyType::Padding, Vec4(10, 5), CStateFlag::Default, CSubControl::Tab);

		auto& buttonSelector = gStyleManager->AddStyleGroup("CButton");
		buttonSelector.AddProperty(CStylePropertyType::Background, Color(0.25f, 0.25f, 0.25f, 1.00f), CStateFlag::Default);
		buttonSelector.AddProperty(CStylePropertyType::Background, Color(0.38f, 0.38f, 0.38f, 1.00f), CStateFlag::Hovered);
		buttonSelector.AddProperty(CStylePropertyType::Background, Color(0.67f, 0.67f, 0.67f, 0.39f), CStateFlag::Pressed);

		auto& tableSelector = gStyleManager->GetStyleGroup("CTableView");
		tableSelector.AddProperty(CStylePropertyType::Background, Color(0.20f, 0.20f, 0.20f, 1.0f), CStateFlag::Default, CSubControl::Header);
		tableSelector.AddProperty(CStylePropertyType::Background, Color(0.23f, 0.23f, 0.23f, 1.0f), CStateFlag::Hovered, CSubControl::Header);
		tableSelector.AddProperty(CStylePropertyType::Background, Color(0.26f, 0.26f, 0.26f, 1.0f), CStateFlag::Pressed, CSubControl::Header);
		tableSelector.AddProperty(CStylePropertyType::Background, Color(0.00f, 0.00f, 0.00f, 0.00f), CStateFlag::Default, CSubControl::TableRowEven);
		tableSelector.AddProperty(CStylePropertyType::Background, Color(1.00f, 1.00f, 1.00f, 0.06f), CStateFlag::Default, CSubControl::TableRowOdd);
		tableSelector.AddProperty(CStylePropertyType::Background, Color::FromRGBA32(30, 30, 30), CStateFlag::Default, CSubControl::TableBorderInner);

		auto& collapsibleSelector = gStyleManager->GetStyleGroup("CCollapsibleSection");
		collapsibleSelector.AddProperty(CStylePropertyType::BorderColor, Color::FromRGBA32(30, 30, 30), CStateFlag::Default, CSubControl::Header);
		collapsibleSelector.AddProperty(CStylePropertyType::Padding, Vec4(4, 4, 4, 4), CStateFlag::Default, CSubControl::Header);
		collapsibleSelector.AddProperty(CStylePropertyType::BorderRadius, Vec4(), CStateFlag::Default, CSubControl::Header);

		auto& textInputSelector = gStyleManager->GetStyleGroup(".TextField");
		textInputSelector.AddProperty(CStylePropertyType::Padding, Vec4(5, 6, 5, 6));
		textInputSelector.AddProperty(CStylePropertyType::Width, CStyleValue(100, true));
		textInputSelector.AddProperty(CStylePropertyType::BorderRadius, Vec4(1, 1, 1, 1) * 1.5f);
	}

	void EditorStyles::GetDefaultFont(unsigned char** outFont, unsigned int* outLength)
	{
		*outFont = OpenSans_VariableFont_ttf;
		*outLength = OpenSans_VariableFont_len;
	}

} // namespace CE::Editor

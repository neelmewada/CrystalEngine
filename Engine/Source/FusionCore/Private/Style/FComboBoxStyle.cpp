#include "FusionCore.h"

namespace CE
{
	FComboBoxStyle::FComboBoxStyle()
	{
	}

	SubClass<FWidget> FComboBoxStyle::GetWidgetClass() const
	{
		return FComboBox::StaticType();
	}

	FComboBoxPlainStyle::FComboBoxPlainStyle()
	{

	}

	void FComboBoxPlainStyle::MakeStyle(FWidget& widget)
	{
		FComboBox& comboBox = widget.As<FComboBox>();

		FBrush backgroundBrush = background;
		Color border = borderColor;

		if (comboBox.IsPressed() && comboBox.IsHovered())
			border = pressedBorderColor;
		else if (comboBox.IsHovered())
			border = hoverBorderColor;

		comboBox
			.Background(background)
			.BorderColor(borderColor)
			.BackgroundShape(FRoundedRectangle(cornerRadius))
			.BorderWidth(borderWidth)
			.BorderColor(border)
			;

		if (comboBox.GetChild() == nullptr)
			return;

		FWidget& child = *comboBox.GetChild();
		f32 childHeight = child.GetComputedSize().y;

		child
			.Translation(Vec2(0, comboBox.IsPressed() && comboBox.IsHovered() ? childHeight * 0.1f : 0))
			;
	}

} // namespace CE

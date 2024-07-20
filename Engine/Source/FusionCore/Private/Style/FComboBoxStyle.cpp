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

	FComboBoxPopupStyle::FComboBoxPopupStyle()
	{
	}

	SubClass<FWidget> FComboBoxPopupStyle::GetWidgetClass() const
	{
		return FComboBoxPopup::StaticType();
	}

	FComboBoxItemStyle::FComboBoxItemStyle()
	{
	}

	SubClass<FWidget> FComboBoxItemStyle::GetWidgetClass() const
	{
		return FComboBoxItem::StaticType();
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

	FComboBoxPopupPlainStyle::FComboBoxPopupPlainStyle()
	{
	}

	void FComboBoxPopupPlainStyle::MakeStyle(FWidget& widget)
	{
		FComboBoxPopup& popup = widget.As<FComboBoxPopup>();

		popup
			.Background(background)
			.BorderWidth(borderWidth)
			.BorderColor(borderColor)
			;
	}

	FComboBoxItemPlainStyle::FComboBoxItemPlainStyle()
	{
	}

	void FComboBoxItemPlainStyle::MakeStyle(FWidget& widget)
	{
		FComboBoxItem& item = widget.As<FComboBoxItem>();

		Color bg = background;
		Color border = borderColor;
		FShapeType shapeType = shape;

		if (item.IsHovered())
		{
			bg = hoverBackground;
			border = hoverBorderColor;
			shapeType = hoverShape;
		}
		else if (item.IsSelected())
		{
			bg = selectedBackground;
			border = selectedBorderColor;
			shapeType = selectedShape;
		}

		
		item
			.Background(bg)
			.Border(border, borderWidth)
			.BackgroundShape(shapeType)
			;

		if (shapeType == FShapeType::RoundedRect)
		{
			item
				.BackgroundShape(FRoundedRectangle(shapeCornerRadius));
		}

	}

} // namespace CE
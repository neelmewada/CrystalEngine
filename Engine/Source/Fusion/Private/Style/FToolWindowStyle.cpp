#include "Fusion.h"

namespace CE
{

	FToolWindowStyle::FToolWindowStyle()
	{
		
	}

	SubClass<FWidget> FToolWindowStyle::GetWidgetClass() const
	{
		return FToolWindow::StaticType();
	}

	void FToolWindowStyle::MakeStyle(FWidget& widget)
	{
		FToolWindow& window = widget.As<FToolWindow>();

		Vec4 newPadding = window.Padding();

		if (padding.left >= 0)
		{
			newPadding.left = padding.left;
		}

		if (padding.top >= 0)
		{
			newPadding.top = padding.top;
		}

		if (padding.right >= 0)
		{
			newPadding.right = padding.right;
		}

		if (padding.bottom >= 0)
		{
			newPadding.bottom = padding.bottom;
		}

		window
			.TitleBarBackground(titleBarBackground)
			.Background(background)
			.BorderColor(borderColor)
			.BorderWidth(borderWidth)
			.Padding(newPadding)
		;
	}


}

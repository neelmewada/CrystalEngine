#pragma once

namespace CE
{
	CLASS()
	class FUSION_API FToolWindowStyle : public FStyle
	{
		CE_CLASS(FToolWindowStyle, FStyle)
	public:

		FToolWindowStyle();

		SubClass<FWidget> GetWidgetClass() const override;

		void MakeStyle(FWidget& widget) override;

	protected:

	public:

		FIELD()
		FBrush background = Color::RGBA(36, 36, 36);

		FIELD()
		Color borderColor = Color::RGBA(15, 15, 15);

		FIELD()
		f32 borderWidth = 1.0f;

		FIELD()
		Vec4 padding = Vec4(-1, -1, -1, -1);

		FIELD()
		FBrush titleBarBackground = Color::RGBA(26, 26, 26);

	};

} // namespace CE

#include "FToolWindowStyle.rtti.h"
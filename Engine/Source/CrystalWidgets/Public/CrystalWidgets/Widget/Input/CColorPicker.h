#pragma once

namespace CE::Widgets
{
	DECLARE_SCRIPT_EVENT(CColorEvent, const Color& color);
	DECLARE_SCRIPT_EVENT(CColorHSVEvent, f32 h, f32 s, f32 v);
	DECLARE_SCRIPT_EVENT(CColorPickerPositionEvent, Vec2 normalizedPosition);

	CLASS()
	class CRYSTALWIDGETS_API CColorPicker : public CWidget
	{
		CE_CLASS(CColorPicker, CWidget)
	public:

		CColorPicker();

		Vec2 CalculateIntrinsicSize(f32 width, f32 height) override;

		Vec2 GetNormalizedPosition() const { return normalizedPosition; }

		void SetNormalizedPosition(Vec2 newPosition);

		// - Events -

		FIELD()
		CColorPickerPositionEvent onPositionChanged{};

		FIELD()
		CColorEvent onColorChanged{};

		FIELD()
		CColorHSVEvent onHSVColorChanged{};

	protected:

		void Construct() override;

		void OnPaintEarly(CPaintEvent* paintEvent) override;

		void OnPaint(CPaintEvent* paintEvent) override;

		void HandleEvent(CEvent* event) override;

		FIELD()
		Vec2 normalizedPosition = Vec2();

	};

} // namespace CE::Widgets

#include "CColorPicker.rtti.h"
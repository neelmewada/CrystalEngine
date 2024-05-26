#pragma once

namespace CE::Widgets
{
	CLASS()
	class CRYSTALWIDGETS_API CColorPicker : public CWidget
	{
		CE_CLASS(CColorPicker, CWidget)
	public:

		CColorPicker();

		Vec2 CalculateIntrinsicSize(f32 width, f32 height) override;

		Vec2 GetNormalizedPosition() const { return normalizedPosition; }

		void SetNormalizedPosition(Vec2 newPosition);

		// - Signals -

		// Params: Vec2 normalizedPosition
		CE_SIGNAL(OnPositionChanged, Vec2);

		CE_SIGNAL(OnColorChanged, Color);

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
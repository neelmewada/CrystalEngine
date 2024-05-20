#pragma once

namespace CE::Widgets
{
	class CSplitView;

	CLASS()
	class CRYSTALWIDGETS_API CSplitViewContainer : public CWidget
	{
		CE_CLASS(CSplitViewContainer, CWidget)
	public:

		CSplitViewContainer();

		virtual ~CSplitViewContainer();

		bool IsLayoutCalculationRoot() override;

		Vec2 CalculateIntrinsicSize(f32 width, f32 height) override;

		Vec2 GetComputedLayoutTopLeft() override;
		Vec2 GetComputedLayoutSize() override;

		f32 GetSplitRatio() const { return splitRatio; }

	private:

		FIELD()
		CSplitView* splitView = nullptr;

		FIELD()
		f32 splitRatio = 0.2f;

		friend class CSplitView;
	};

} // namespace CE::Widgets

#include "CSplitViewContainer.rtti.h"
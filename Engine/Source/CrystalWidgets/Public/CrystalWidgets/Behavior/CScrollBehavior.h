#pragma once

namespace CE::Widgets
{

    CLASS()
	class CRYSTALWIDGETS_API CScrollBehavior : public CBehavior
	{
		CE_CLASS(CScrollBehavior, CBehavior)
	public:

		CScrollBehavior();
		virtual ~CScrollBehavior();

		Rect GetVerticalScrollBarRect();

		bool IsVerticalScrollVisible();

	protected:

		void HandleEvent(CEvent* event) override;

		void OnPaintOverlay(CPainter* painter) override;

		Vec4 GetExtraRootPadding() override;

    private:

		bool isVerticalScrollHovered = false;
		bool isVerticalScrollPressed = false;

	};

    
} // namespace CE::Widgets

#include "CScrollBehavior.rtti.h"
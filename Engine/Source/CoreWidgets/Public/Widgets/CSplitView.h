#pragma once

namespace CE::Widgets
{
	CLASS()
	class COREWIDGETS_API CSplitView : public CWidget
	{
		CE_CLASS(CSplitView, CWidget)
	public:

		CSplitView();
		virtual ~CSplitView();

		bool IsLayoutCalculationRoot() override { return true; }

		void Construct() override;

		Vec2 CalculateIntrinsicContentSize(f32 width, f32 height) override;

		FORCE_INLINE CLayoutGroup* GetLeft() const { return left; }
		FORCE_INLINE CLayoutGroup* GetRight() const { return right; }

	protected:

		void OnDrawGUI() override;

		FIELD()
		CLayoutGroup* left = nullptr;

		FIELD()
		CLayoutGroup* right = nullptr;

		FIELD(ReadOnly)
		f32 split = 0.5f;
	};
    
} // namespace CE::Widgets

#include "CSplitView.rtti.h"

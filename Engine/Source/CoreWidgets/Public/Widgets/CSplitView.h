#pragma once

namespace CE::Widgets
{
	CLASS()
	class COREWIDGETS_API CSplitViewContainer : public CLayoutGroup
	{
		CE_CLASS(CSplitViewContainer, CLayoutGroup)
	public:

		CSplitViewContainer();
		virtual ~CSplitViewContainer();

		bool IsLayoutCalculationRoot() override final { return true; }

	corewidgets_protected_internal:

		void Construct() override;

		void OnDrawGUI() override;

		Vec2 columnSize = Vec2();
	};

	CLASS()
	class COREWIDGETS_API CSplitView : public CWidget
	{
		CE_CLASS(CSplitView, CWidget)
	public:

		CSplitView();
		virtual ~CSplitView();

		void Construct() override;

		Vec2 CalculateIntrinsicContentSize(f32 width, f32 height) override;

		FORCE_INLINE CLayoutGroup* GetLeft() const { return left; }
		FORCE_INLINE CLayoutGroup* GetRight() const { return right; }
	
	corewidgets_protected_internal:

		void OnDrawGUI() override;

		FIELD()
		CSplitViewContainer* left = nullptr;

		FIELD()
		CSplitViewContainer* right = nullptr;

		FIELD(ReadOnly)
		f32 split = 0.5f;

		FIELD(ReadOnly)
		f32 leftSize = 0;

		FIELD(ReadOnly)
		f32 rightSize = 0;
	};
    
} // namespace CE::Widgets

#include "CSplitView.rtti.h"

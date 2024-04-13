#pragma once


namespace CE::Widgets
{

	CLASS()
	class CRYSTALWIDGETS_API CDockSpace : public CWindow
	{
		CE_CLASS(CDockSpace, CWindow)
	public:

		CDockSpace();
		virtual ~CDockSpace();

		bool IsLayoutCalculationRoot() override { return dockType == CDockType::Major; }

		CDockSplitView* GetRootDockSplit() const { return dockSplits.Top(); }

		bool Split(CDockSplitView* originalSplitView, f32 splitRatio, CDockSplitDirection splitDirection, Name splitName1 = "", Name splitName2 = "");

		bool Split(f32 splitRatio, CDockSplitDirection splitDirection, Name splitName1 = "", Name splitName2 = "");

		CDockType GetDockType() const { return dockType; }

		void SetDockType(CDockType dockType) { this->dockType = dockType; }

	protected:

		void HandleEvent(CEvent* event) override;

		void OnPlatformWindowSet() override;

		void OnSubobjectAttached(Object* object) override;
		void OnSubobjectDetached(Object* object) override;

		void OnPaint(CPaintEvent* paintEvent) override;

		bool WindowHitTest(PlatformWindow* window, Vec2 position) override;

		FIELD()
		Array<CDockSplitView*> dockSplits{};

		FIELD()
		CDockType dockType = CDockType::Major;

		CMenu* curMenuPopup = nullptr;

		Array<Rect> menuItemRects{};
		int hoveredMenuItem = -1;
		int activeMenuItem = -1;

		b8 isDetachedMode = false;
	};

} // namespace CE::Widgets

#include "CDockSpace.rtti.h"
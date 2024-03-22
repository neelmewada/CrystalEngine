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

		CDockSplitView* GetLastDockSplit() const { return dockSplits.Top(); }

		bool Split(CDockSplitView* originalSplitView, f32 splitRatio, CDockSplitDirection splitDirection);

		bool Split(f32 splitRatio, CDockSplitDirection splitDirection);

		CDockType GetDockType() const { return dockType; }

	private:

		void HandleEvent(CEvent* event) override;

		void OnPlatformWindowSet() override;

		void OnSubobjectAttached(Object* object) override;
		void OnSubobjectDetached(Object* object) override;

		void OnPaint(CPaintEvent* paintEvent) override;

		bool WindowHitTest(PlatformWindow* window, Vec2 position);

		FIELD()
		Array<CDockSplitView*> dockSplits{};

		FIELD()
		CDockType dockType = CDockType::Major;

	};

} // namespace CE::Widgets

#include "CDockSpace.rtti.h"
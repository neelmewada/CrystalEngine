#pragma once

namespace CE::Widgets
{

	class CDockWindow;
	class CDockSpace;

	ENUM()
	enum class CDockSplitDirection
	{
		Vertical = 0,
		Horizontal
	};
	ENUM_CLASS(CDockSplitDirection);

	ENUM()
	enum class CDockType
	{
		Major = 0,
		Minor
	};
	ENUM_CLASS(CDockType);

	CLASS()
	class CRYSTALWIDGETS_API CDockSplitView : public CWidget
	{
		CE_CLASS(CDockSplitView, CWidget)
	public:

		CDockSplitView();
		virtual ~CDockSplitView();

		CDockSplitDirection GetSplitDirection() const { return splitDirection; }

		f32 GetSplitRatio() const { return splitRatio; }

		CDockSplitView* FindSplit(Name splitName);

		u32 GetSplitCount() const { return children.GetSize(); }
		CDockSplitView* GetSplit(u32 index) const { return children[index]; }

		CDockWindow* GetActiveWindow();

		bool IsLayoutCalculationRoot() override { return true; }

		bool IsSubWidgetAllowed(Class* subWidgetClass) override;

		virtual Vec2 GetComputedLayoutTopLeft() override;
		virtual Vec2 GetComputedLayoutSize() override;

	private:

		void OnSubobjectAttached(Object* subobject) override;
		void OnSubobjectDetached(Object* subobject) override;

		void HandleEvent(CEvent* event) override;

		void OnPaint(CPaintEvent* paintEvent) override;

		FIELD()
		CDockSplitView* parentSplitView = nullptr;

		FIELD()
		Array<CDockSplitView*> children{};

		FIELD()
		Array<CDockWindow*> dockedWindows{};

		FIELD()
		CDockSplitDirection splitDirection = CDockSplitDirection::Vertical;

		FIELD()
		f32 splitRatio = 1.0f;

		FIELD()
		CDockSpace* dockSpace = nullptr;

		int draggedSplitIdx = -1;
		int hoveredSplitIdx = -1;
		int selectedTab = 0;
		b8 isLeftMousePressedOnTab = false;
		b8 hasPushedResizeCursor = false;

		struct TabData
		{
			Rect rect{};
		};

		Array<TabData> tabs{};

		f32 totalDeltaX = 0;

		friend class CDockSpace;
	};

} // namespace CE::Widgets

#include "CDockSplitView.rtti.h"
#pragma once

namespace CE::Widgets
{
	DECLARE_SCRIPT_EVENT(CSplitViewEvent, CSplitView* sender, int splitterIndex);

	CLASS()
	class CRYSTALWIDGETS_API CSplitView : public CWidget
	{
		CE_CLASS(CSplitView, CWidget)
	public:

		CSplitView();
		virtual ~CSplitView();

		bool IsSubWidgetAllowed(Class* subWidgetClass) override;

		void SetOrientation(COrientation orientation);

		COrientation GetOrientation() const { return orientation; }

		bool IsLayoutCalculationRoot() override;

		Vec2 CalculateIntrinsicSize(f32 width, f32 height) override;

		void AddSplit(f32 ratio);

		u32 GetContainerCount() const { return containers.GetSize(); }

		CSplitViewContainer* GetContainer(u32 index) const { return containers[index]; }

		Vec2 GetAvailableSizeForChild(CWidget* childWidget) override;

		int GetIndexOfContainer(CSplitViewContainer* container) const;

		void SetContainerSplitRatio(CSplitViewContainer* container, f32 newSplitRatio);

		// - Events -

		FIELD()
		CSplitViewEvent onSplitterDragged{};

	protected:

		void OnBeforeComputeStyle() override;

		void OnPaint(CPaintEvent* paintEvent) override;

		void HandleEvent(CEvent* event) override;

		void OnSubobjectAttached(Object* subobject) override;

		void OnSubobjectDetached(Object* subobject) override;

		void UpdateSplits();

		FIELD()
		COrientation orientation = COrientation::Horizontal;

		FIELD()
		Array<CSplitViewContainer*> containers{};

	private:

		Color splitterColor = Color();
		Color splitterActiveColor = Color();

		int draggedSplitIdx = -1;
		int hoveredSplitIdx = -1;
		b8 hasPushedResizeCursor = false;

		friend class CSplitViewContainer;
	};

} // namespace CE::Widgets

#include "CSplitView.rtti.h"
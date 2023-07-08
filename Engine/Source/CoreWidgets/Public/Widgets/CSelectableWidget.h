#pragma once

namespace CE::Widgets
{

	CLASS()
	class COREWIDGETS_API CSelectableWidget : public CWidget
	{
		CE_CLASS(CSelectableWidget, CWidget)
	public:

		CSelectableWidget();
		virtual ~CSelectableWidget();

		void Construct() override;

		void OnAfterComputeStyle() override;

		bool IsContainer() override { return true; }

		inline bool IsSelected() const { return isSelected; }

		bool Select();

	protected:

		void OnDrawGUI() override;

		void HandleEvent(CEvent* event) override;

	protected:

		FIELD()
		b8 isSelected = false;

		GUI::GuiStyleState hoveredState{};
		GUI::GuiStyleState pressedState{};
		GUI::GuiStyleState activeState{};

		friend class CSelectableGroup;
	};
    
} // namespace CE::Widgets

#include "CSelectableWidget.rtti.h"

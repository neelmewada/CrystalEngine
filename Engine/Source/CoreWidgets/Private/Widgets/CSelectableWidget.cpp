#include "CoreWidgets.h"

namespace CE::Widgets
{
	CSelectableWidget::CSelectableWidget()
	{
		
	}

	CSelectableWidget::~CSelectableWidget()
	{

	}

	void CSelectableWidget::Construct()
	{
		Super::Construct();

		AddStyleClass("Selectable");
	}

	void CSelectableWidget::OnAfterComputeStyle()
	{
		Super::OnAfterComputeStyle();

		hoveredState = defaultStyleState;
		pressedState = defaultStyleState;
		activeState = defaultStyleState;

		auto hoveredStyle = stylesheet->SelectStyle(this, CStateFlag::Hovered);
		for (const auto& [property, styleValue] : hoveredStyle.properties)
		{
			LoadGuiStyleStateProperty(property, styleValue, hoveredState);
		}

		auto pressedStyle = stylesheet->SelectStyle(this, CStateFlag::Pressed);
		for (const auto& [property, styleValue] : pressedStyle.properties)
		{
			LoadGuiStyleStateProperty(property, styleValue, pressedState);
		}

		auto activeStyle = stylesheet->SelectStyle(this, CStateFlag::Active);
		for (const auto& [property, styleValue] : activeStyle.properties)
		{
			LoadGuiStyleStateProperty(property, styleValue, activeState);
		}
	}

	bool CSelectableWidget::Select()
	{
		if (GetOwner() != nullptr && GetOwner()->GetClass()->IsSubclassOf<CSelectableGroup>())
		{
			CSelectableGroup* parent = (CSelectableGroup*)GetOwner();
			parent->OnWidgetSelected(this);
		}
		else
		{
			isSelected = true;
		}
		return true;
	}

	void CSelectableWidget::OnDrawGUI()
	{
		auto rect = GetComputedLayoutRect();
		auto padding = GetComputedLayoutPadding();

		DrawDefaultBackground();

		bool selected = isSelected;
		selected = GUI::Selectable(rect, GetUuid(), selected, hoveredState, pressedState, activeState);

		if (selected && !isSelected && GetOwner() != nullptr && GetOwner()->GetClass()->IsSubclassOf<CSelectableGroup>())
		{
			isSelected = true;

			CSelectableGroup* parent = (CSelectableGroup*)GetOwner();
			parent->OnWidgetSelected(this);
		}

		if (isSelected && !EnumHasAnyFlags(stateFlags, CStateFlag::Active))
		{
			stateFlags = CStateFlag::Active;
			SetNeedsStyle();
		}
		
		if (!isSelected && EnumHasAnyFlags(stateFlags, CStateFlag::Active))
		{
			EnumRemoveFlags(stateFlags, CStateFlag::Active);
			SetNeedsStyle();
		}

		PollEvents();

		GUI::PushChildCoordinateSpace(rect);

		for (auto child : attachedWidgets)
		{
			child->Render();
		}

		GUI::PopChildCoordinateSpace();
	}

	void CSelectableWidget::HandleEvent(CEvent* event)
	{

		Super::HandleEvent(event);
	}

} // namespace CE::Widgets

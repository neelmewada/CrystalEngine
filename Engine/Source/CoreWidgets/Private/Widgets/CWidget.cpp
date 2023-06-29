
#include "CoreWidgets.h"

namespace CE::Widgets
{
	YGSize CWidget::MeasureFunctionCallback(YGNodeRef nodeRef, float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode)
	{
		CWidget* widget = (CWidget*)YGNodeGetContext(nodeRef);
		if (widget == nullptr)
			return YGSize();

		auto size = widget->CalculateIntrinsicContentSize(width, height);
        return YGSize{size.x, size.y};
	}

	CWidget::CWidget()
	{
		node = YGNodeNew();
		YGNodeSetContext(node, this);
		YGNodeSetMeasureFunc(node, MeasureFunctionCallback);
	}

	CWidget::~CWidget()
	{
		YGNodeFree(node);
		node = nullptr;
	}

	void CWidget::Construct()
	{
		
	}

	void CWidget::LoadGuiStyleStateProperty(CStylePropertyType property, const CStyleValue& styleValue, GUI::GuiStyleState& outState)
	{
		if (property == CStylePropertyType::Background && styleValue.IsColor())
		{
			outState.background = styleValue.color;
		}
		else if (property == CStylePropertyType::ForegroundColor && styleValue.IsColor())
		{
			outState.foreground = styleValue.color;
		}
		else if (property == CStylePropertyType::BorderRadius)
		{
			if (styleValue.IsSingle())
				outState.borderRadius = Vec4(1, 1, 1, 1) * styleValue.single;
			else if (styleValue.IsVector())
				outState.borderRadius = styleValue.vector;
		}
		else if (property == CStylePropertyType::BorderColor && styleValue.IsColor())
		{
			outState.borderColor = styleValue.color;
		}
		else if (property == CStylePropertyType::BorderWidth && styleValue.IsSingle())
		{
			outState.borderThickness = styleValue.single;
		}
		else if (property == CStylePropertyType::TextAlign && styleValue.IsEnum())
		{
			outState.textAlign = (GUI::TextAlign)styleValue.enumValue.x;
		}
	}

	void CWidget::UpdateStyleIfNeeded()
	{
		if (!needsStyle && !style.IsDirty())
			return;

		node->setStyle({}); // Clear style

		for (auto& [property, variants] : style.properties)
		{
			const auto& value = variants.Get();

			// Non-Yoga properties
			if (value.IsValid()) // Default state
			{
				LoadGuiStyleStateProperty(property, value, defaultStyleState);
			}

			const auto& hoveredState = variants.Get(CStateFlag::Hovered);
			if (hoveredState.IsValid())
			{
				LoadGuiStyleStateProperty(property, hoveredState, hoveredStyleState);
			}
			else
			{
				LoadGuiStyleStateProperty(property, value, hoveredStyleState);
			}

			const auto& pressedState = variants.Get(CStateFlag::Pressed);
			if (pressedState.IsValid())
			{
				LoadGuiStyleStateProperty(property, pressedState, pressedStyleState);
			}
			else
			{
				LoadGuiStyleStateProperty(property, value, pressedStyleState);
			}
			
			if (!value.IsValid())
				continue;

			// Yoga Properties
			if (property == CStylePropertyType::AlignContent && value.IsEnum())
			{
				YGNodeStyleSetAlignContent(node, (YGAlign)value.enumValue.x);
			}
			else if (property == CStylePropertyType::AlignItems && value.IsEnum())
			{
				YGNodeStyleSetAlignItems(node, (YGAlign)value.enumValue.x);
			}
			else if (property == CStylePropertyType::AlignSelf && value.IsEnum())
			{
				YGNodeStyleSetAlignSelf(node, (YGAlign)value.enumValue.x);
			}
			else if (property == CStylePropertyType::JustifyContent && value.IsEnum())
			{
				YGNodeStyleSetJustifyContent(node, (YGJustify)value.enumValue.x);
			}
			else if (property == CStylePropertyType::Flex && value.IsSingle())
			{
				YGNodeStyleSetFlex(node, value.single);
			}
			else if (property == CStylePropertyType::FlexBasis)
			{
				if (value.IsAutoValue())
				{
					YGNodeStyleSetFlexBasisAuto(node);
				}
				else if (value.IsSingle())
				{
					if (value.IsPercentValue())
						YGNodeStyleSetFlexBasisPercent(node, value.single);
					else
						YGNodeStyleSetFlexBasis(node, value.single);
				}
			}
			else if (property == CStylePropertyType::FlexWrap && value.IsEnum())
			{
				YGNodeStyleSetFlexWrap(node, (YGWrap)value.enumValue.x);
			}
			else if (property == CStylePropertyType::FlexGrow && value.IsSingle())
			{
				YGNodeStyleSetFlexGrow(node, value.single);
			}
			else if (property == CStylePropertyType::FlexShrink && value.IsSingle())
			{
				YGNodeStyleSetFlexShrink(node, value.single);
			}
			else if (property == CStylePropertyType::FlexDirection && value.IsEnum())
			{
				YGNodeStyleSetFlexDirection(node, (YGFlexDirection)value.enumValue.x);
			}
			else if (property == CStylePropertyType::RowGap && value.IsSingle())
			{
				YGNodeStyleSetGap(node, YGGutterRow, value.single);
			}
			else if (property == CStylePropertyType::ColumnGap && value.IsSingle())
			{
				YGNodeStyleSetGap(node, YGGutterColumn, value.single);
			}
			else if (property == CStylePropertyType::Margin)
			{
				if (value.IsEnum())
				{
					YGNodeStyleSetMarginAuto(node, YGEdgeAll);
				}
				else if (value.IsSingle())
				{
					if (value.enumValue.x == CStyleValue::Percent)
						YGNodeStyleSetMarginPercent(node, YGEdgeAll, value.single);
					else if (value.enumValue.x == CStyleValue::Auto)
						YGNodeStyleSetMarginAuto(node, YGEdgeAll);
					else
						YGNodeStyleSetMargin(node, YGEdgeAll, value.single);
				}
				else if (value.IsVector())
				{
					// Left
					if (value.enumValue.x == CStyleValue::None)
						YGNodeStyleSetMargin(node, YGEdgeLeft, value.vector.x);
					else if (value.enumValue.x == CStyleValue::Percent)
						YGNodeStyleSetMarginPercent(node, YGEdgeLeft, value.vector.x);
					else if (value.enumValue.x == CStyleValue::Auto)
						YGNodeStyleSetMarginAuto(node, YGEdgeLeft);
					// Top
					if (value.enumValue.y == CStyleValue::None)
						YGNodeStyleSetMargin(node, YGEdgeTop, value.vector.y);
					else if (value.enumValue.y == CStyleValue::Percent)
						YGNodeStyleSetMarginPercent(node, YGEdgeTop, value.vector.y);
					else if (value.enumValue.y == CStyleValue::Auto)
						YGNodeStyleSetMarginAuto(node, YGEdgeTop);
					// Right
					if (value.enumValue.z == CStyleValue::None)
						YGNodeStyleSetMargin(node, YGEdgeRight, value.vector.z);
					else if (value.enumValue.z == CStyleValue::Percent)
						YGNodeStyleSetMarginPercent(node, YGEdgeRight, value.vector.z);
					else if (value.enumValue.z == CStyleValue::Auto)
						YGNodeStyleSetMarginAuto(node, YGEdgeRight);
					// Bottom
					if (value.enumValue.w == CStyleValue::None)
						YGNodeStyleSetMargin(node, YGEdgeBottom, value.vector.w);
					else if (value.enumValue.w == CStyleValue::Percent)
						YGNodeStyleSetMarginPercent(node, YGEdgeBottom, value.vector.w);
					else if (value.enumValue.w == CStyleValue::Auto)
						YGNodeStyleSetMarginAuto(node, YGEdgeBottom);
				}
			}
			else if (property == CStylePropertyType::Padding)
			{
				if (value.IsSingle())
				{
					if (value.enumValue.x == CStyleValue::Percent)
						YGNodeStyleSetPadding(node, YGEdgeAll, value.single);
					else
						YGNodeStyleSetPadding(node, YGEdgeAll, value.single);
				}
				else if (value.IsVector())
				{
					// Left
					if (value.enumValue.x == CStyleValue::Percent)
						YGNodeStyleSetPadding(node, YGEdgeLeft, value.vector.x);
					else
						YGNodeStyleSetPadding(node, YGEdgeLeft, value.vector.x);
					// Top
					if (value.enumValue.y == CStyleValue::Percent)
						YGNodeStyleSetPadding(node, YGEdgeTop, value.vector.y);
					else
						YGNodeStyleSetPadding(node, YGEdgeTop, value.vector.y);
					// Right
					if (value.enumValue.z == CStyleValue::Percent)
						YGNodeStyleSetPadding(node, YGEdgeRight, value.vector.z);
					else
						YGNodeStyleSetPadding(node, YGEdgeRight, value.vector.z);
					// Bottom
					if (value.enumValue.w == CStyleValue::Percent)
						YGNodeStyleSetPadding(node, YGEdgeBottom, value.vector.w);
					else
						YGNodeStyleSetPadding(node, YGEdgeBottom, value.vector.w);
				}
			}
			else if (property == CStylePropertyType::BorderWidth)
			{
				if (value.IsSingle())
				{
					YGNodeStyleSetBorder(node, YGEdgeAll, value.single);
				}
				else if (value.IsVector())
				{
					YGNodeStyleSetBorder(node, YGEdgeLeft, value.vector.left);
					YGNodeStyleSetBorder(node, YGEdgeTop, value.vector.top);
					YGNodeStyleSetBorder(node, YGEdgeRight, value.vector.right);
					YGNodeStyleSetBorder(node, YGEdgeBottom, value.vector.bottom);
				}
			}
			else if (property == CStylePropertyType::Width)
			{
				if (value.IsAutoValue())
				{
					YGNodeStyleSetWidthAuto(node);
				}
				else if (value.IsSingle())
				{
					if (value.enumValue.x == CStyleValue::Percent)
						YGNodeStyleSetWidthPercent(node, value.single);
					else
						YGNodeStyleSetWidth(node, value.single);
				}
			}
			else if (property == CStylePropertyType::MinWidth)
			{
				if (value.IsSingle())
				{
					if (value.enumValue.x == CStyleValue::Percent)
						YGNodeStyleSetMinWidthPercent(node, value.single);
					else
						YGNodeStyleSetMinWidth(node, value.single);
				}
			}
			else if (property == CStylePropertyType::MaxWidth)
			{
				if (value.IsSingle())
				{
					if (value.enumValue.x == CStyleValue::Percent)
						YGNodeStyleSetMaxWidthPercent(node, value.single);
					else
						YGNodeStyleSetMaxWidth(node, value.single);
				}
			}
			else if (property == CStylePropertyType::Height)
			{
				if (value.IsAutoValue())
				{
					YGNodeStyleSetHeightAuto(node);
				}
				else if (value.IsSingle())
				{
					if (value.enumValue.x == CStyleValue::Percent)
						YGNodeStyleSetHeightPercent(node, value.single);
					else
						YGNodeStyleSetHeight(node, value.single);
				}
			}
			else if (property == CStylePropertyType::MinHeight)
			{
				if (value.IsSingle())
				{
					if (value.enumValue.x == CStyleValue::Percent)
						YGNodeStyleSetMinHeightPercent(node, value.single);
					else
						YGNodeStyleSetMinHeight(node, value.single);
				}
			}
			else if (property == CStylePropertyType::MaxHeight)
			{
				if (value.IsSingle())
				{
					if (value.enumValue.x == CStyleValue::Percent)
						YGNodeStyleSetMaxHeightPercent(node, value.single);
					else
						YGNodeStyleSetMaxHeight(node, value.single);
				}
			}
		}

		OnComputeStyle();

		needsStyle = false;
		needsLayout = true;

		style.SetDirty(false);

		if (IsContainer())
		{
			for (CWidget* subWidget : attachedWidgets)
			{
				if (subWidget == nullptr || subWidget == this)
					continue;
				subWidget->SetNeedsStyle();
				subWidget->UpdateStyleIfNeeded();
			}
		}
	}

	void CWidget::UpdateLayoutIfNeeded()
	{
		if (!needsLayout)
			return;
        
        if (IsWindow())
        {
			CWindow* window = (CWindow*)this;
			if (!window->IsDockSpaceWindow())
			{
				Vec2 size = CalculateIntrinsicContentSize(YGUndefined, YGUndefined);
				if (size.x <= 0) size.x = YGUndefined;
				if (size.y <= 0) size.y = YGUndefined;
				YGNodeCalculateLayout(node, size.x, size.y, YGDirectionLTR);
			}
        }

		needsLayout = false;
	}

	void CWidget::OnAttachedTo(CWidget* parent)
	{
		if (inheritedPropertiesInitialized) // Inherited properties can only be initialized once
			return;

		for (auto& [property, variants] : style.properties)
		{
			auto& defaultState = variants.Get();

			if (defaultState.IsValid())
			{
				if (defaultState.enumValue.x == CStyleValue::Inherited)
				{
					const auto& parentVariants = parent->style.properties[property];
					const auto& parentDefaultState = parentVariants.Get();

					if (parentDefaultState.IsValid())
					{
						defaultState = parentDefaultState;
						continue;
					}
				}
			}
			else if (CStyle::IsInheritedProperty(property)) // If default state does not exist but property is auto-inheritable
			{
				const auto& parentVariants = parent->style.properties[property];
				const auto& parentDefaultState = parentVariants.Get();

				if (parentDefaultState.IsValid())
				{
					variants.Add(parentDefaultState);
				}
			}
		}

		SetNeedsStyle();

		inheritedPropertiesInitialized = true;
	}

	void CWidget::OnDetachedFrom(CWidget* parent)
	{

	}

	void CWidget::OnSubobjectAttached(Object* subobject)
	{
		if (subobject == nullptr)
			return;
		if (subobject->GetClass()->IsSubclassOf<CWidget>())
		{
			CWidget* subWidget = (CWidget*)subobject;
			attachedWidgets.Add(subWidget);
			OnSubWidgetAttached(subWidget);
			subWidget->OnAttachedTo(this);

			YGNodeSetMeasureFunc(node, nullptr);

			auto childCount = YGNodeGetChildCount(node);
			YGNodeInsertChild(node, subWidget->node, childCount);
		}
	}

	void CWidget::OnSubobjectDetached(Object* subobject)
	{
		if (subobject == nullptr)
			return;
		if (subobject->GetClass()->IsSubclassOf<CWidget>())
		{
			CWidget* subWidget = (CWidget*)subobject;
			attachedWidgets.Remove(subWidget);
			OnSubWidgetDetached(subWidget);
			subWidget->OnDetachedFrom(this);

			auto childCount = YGNodeGetChildCount(node);
			for (int i = 0; i < childCount; i++)
			{
				auto childNodeRef = YGNodeGetChild(node, i);
				CWidget* ctx = (CWidget*)YGNodeGetContext(childNodeRef);
				if (ctx != nullptr && ctx == subWidget)
				{
					YGNodeRemoveChild(node, childNodeRef);
					break;
				}
			}

			childCount = YGNodeGetChildCount(node);
			if (childCount == 0)
			{
				YGNodeSetMeasureFunc(node, MeasureFunctionCallback);
			}
		}
	}

	void CWidget::RenderGUI()
	{
		if (style.IsDirty())
		{
			needsStyle = true;
			needsLayout = true;
		}

		OnDrawGUI();

		UpdateStyleIfNeeded();
		UpdateLayoutIfNeeded();
	}

	void CWidget::PollEvents()
	{
		CWindow* thisWindow = nullptr;
		if (IsWindow())
			thisWindow = (CWindow*)this;

		bool focused = IsWindow() ? GUI::IsWindowFocused(thisWindow->GetTitle(), GUI::FOCUS_ChildWindows | GUI::FOCUS_DockHierarchy) : GUI::IsItemFocused();

		if (focused != isFocused) // Focus changed
		{
			CFocusEvent event{};
			event.name = "FocusChanged";
			event.type = CEventType::FocusChanged;
			event.gotFocus = focused;
			event.sender = this;
			HandleEvent(&event);

			isFocused = focused;
		}

		if (!IsWindow())
		{
			bool hovered = GUI::IsItemHovered();

			// Mouse: Press, Release

			b8 leftMouseDown = GUI::IsMouseDown(GUI::MouseButton::Left);

			if (leftMouseDown != prevLeftMouseDown && hovered) // Left mouse state changed within bounds
			{
				isLeftMousePressedInside = leftMouseDown && hovered;

				if (isLeftMousePressedInside) // Press inside
				{
					CMouseEvent event{};
					event.name = "MousePress";
					event.type = CEventType::MousePress;
					event.mousePos = GUI::GetMousePos();
					event.mouseButton = 0; // Left = 0
					event.prevMousePos = prevHoverPos;
					event.isInside = true;
					event.sender = this;

					HandleEvent(&event);
					prevHoverPos = event.mousePos;
				}
				else // Release inside
				{
					CMouseEvent event{};
					event.name = "MouseRelease";
					event.type = CEventType::MouseRelease;
					event.mousePos = GUI::GetMousePos();
					event.mouseButton = 0; // Left = 0
					event.prevMousePos = prevHoverPos;
					event.isInside = true;
					event.sender = this;

					HandleEvent(&event);
					prevHoverPos = event.mousePos;
				}
			}
			else if (isLeftMousePressedInside && !leftMouseDown && !hovered) // Release outside
			{
				isLeftMousePressedInside = false;

				CMouseEvent event{};
				event.name = "MouseRelease";
				event.type = CEventType::MouseRelease;
				event.mousePos = GUI::GetMousePos();
				event.mouseButton = 0; // Left = 0
				event.prevMousePos = prevHoverPos;
				event.isInside = false;
				event.sender = this;

				HandleEvent(&event);
				prevHoverPos = event.mousePos;
			}

			prevLeftMouseDown = GUI::IsMouseDown(GUI::MouseButton::Left);

			// Mouse: Enter, Move, Leave
			
			if (hovered && !isHovered)
			{
				CMouseEvent event{};
				event.name = "MouseEnter";
				event.type = CEventType::MouseEnter;
				event.mousePos = GUI::GetMousePos();
				event.sender = this;
				HandleEvent(&event);
				isHovered = hovered;
				prevHoverPos = event.mousePos;
			}
			else if (hovered && isHovered)
			{
				auto mousePos = GUI::GetMousePos();
				if (mousePos.x != prevHoverPos.x && mousePos.y != prevHoverPos.y)
				{
					CMouseEvent event{};
					event.name = "MouseMove";
					event.type = CEventType::MouseMove;
					event.mousePos = GUI::GetMousePos();
					event.prevMousePos = prevHoverPos;
					event.sender = this;

					HandleEvent(&event);
					prevHoverPos = mousePos;
				}
			}
			else if (!hovered && isHovered)
			{
				CMouseEvent event{};
				event.name = "MouseLeave";
				event.type = CEventType::MouseLeave;
				event.mousePos = GUI::GetMousePos();
				event.prevMousePos = prevHoverPos;
				event.sender = this;
				HandleEvent(&event);
				isHovered = hovered;
				prevHoverPos = event.mousePos;
			}

			// Mouse Click:

			bool isLeftClicked = GUI::IsItemClicked(GUI::MouseButton::Left);
			bool isRightClicked = GUI::IsItemClicked(GUI::MouseButton::Right);
			bool isMiddleClicked = GUI::IsItemClicked(GUI::MouseButton::Middle);

			if (isLeftClicked || isRightClicked || isMiddleClicked)
			{
				if (GUI::IsItemHovered())
				{
					CMouseEvent event{};
					event.name = "MouseClick";
					event.type = CEventType::MouseButtonClick;
					event.mouseButton = isLeftClicked ? 0 : (isRightClicked ? 1 : 2);
					event.mousePos = GUI::GetMousePos();
					event.prevMousePos = event.mousePos;
					event.sender = this;
					HandleEvent(&event);
				}
			}

			bool isLeftDblClicked = GUI::IsItemDoubleClicked(GUI::MouseButton::Left);
			bool isRightDblClicked = GUI::IsItemDoubleClicked(GUI::MouseButton::Right);
			bool isMiddleDblClicked = GUI::IsItemDoubleClicked(GUI::MouseButton::Middle);

			if (isLeftDblClicked || isRightDblClicked || isMiddleDblClicked)
			{
				if (GUI::IsItemHovered())
				{
					CMouseEvent event{};
					event.name = "MouseDoubleClick";
					event.type = CEventType::MouseButtonDoubleClick;
					event.mouseButton = isLeftDblClicked ? 0 : (isRightDblClicked ? 1 : 2);
					event.mousePos = GUI::GetMousePos();
					event.prevMousePos = event.mousePos;
					event.sender = this;
					HandleEvent(&event);
				}
			}
		}
		else // Window
		{
			thisWindow = (CWindow*)this;

			// Mouse Click:

			bool isLeftClicked = GUI::IsMouseClicked(GUI::MouseButton::Left);
			bool isRightClicked = GUI::IsMouseClicked(GUI::MouseButton::Right);
			bool isMiddleClicked = GUI::IsMouseClicked(GUI::MouseButton::Middle);

			
			
			if (isLeftClicked || isRightClicked || isMiddleClicked)
			{
				bool isWindowHovered = GUI::IsWindowHovered(thisWindow->GetTitle(), GUI::Hovered_ChildWindows | GUI::Hovered_DockHierarchy | GUI::Hovered_NoPopupHierarchy);
				if (isWindowHovered)
				{
					CMouseEvent event{};
					event.name = "MouseClick";
					event.type = CEventType::MouseButtonClick;
					event.mouseButton = isLeftClicked ? 0 : (isRightClicked ? 1 : 2);
					event.mousePos = GUI::GetMousePos();
					event.prevMousePos = event.mousePos;
					event.sender = this;
					HandleEvent(&event);
				}
			}

			bool isLeftDblClicked = GUI::IsMouseDoubleClicked(GUI::MouseButton::Left);
			bool isRightDblClicked = GUI::IsMouseDoubleClicked(GUI::MouseButton::Right);
			bool isMiddleDblClicked = GUI::IsMouseDoubleClicked(GUI::MouseButton::Middle);

			if (isLeftDblClicked || isRightDblClicked || isMiddleDblClicked)
			{
				bool isWindowHovered = GUI::IsWindowHovered(thisWindow->GetTitle(), GUI::Hovered_ChildWindows | GUI::Hovered_DockHierarchy | GUI::Hovered_NoPopupHierarchy);
				if (isWindowHovered)
				{
					CMouseEvent event{};
					event.name = "MouseDoubleClick";
					event.type = CEventType::MouseButtonDoubleClick;
					event.mouseButton = isLeftClicked ? 0 : (isRightClicked ? 1 : 2);
					event.mousePos = GUI::GetMousePos();
					event.prevMousePos = event.mousePos;
					event.sender = this;
					HandleEvent(&event);
				}
			}
		}
	}

    void CWidget::SetWidgetFlags(WidgetFlags flags)
    {
        this->widgetFlags = flags;
    }

    WidgetFlags CWidget::GetWidgetFlags() const
    {
        return widgetFlags;
    }
	
	CWidget* CWidget::GetOwner()
	{
		if (GetOuter() == nullptr)
			return nullptr;
		return DynamicCast<CWidget>(GetOuter());
	}

	CWindow* CWidget::GetOwnerWindow()
	{
		auto owner = GetOwner();
		while (owner != nullptr)
		{
			if (owner->IsWindow())
				return (CWindow*)owner;
			owner = owner->GetOwner();
		}
		return nullptr;
	}

	CWidget* CWidget::FindSubWidget(const Name& name)
	{
		for (auto subWidget : attachedWidgets)
		{
			if (subWidget != nullptr && subWidget->GetName() == name)
				return subWidget;
		}
		return nullptr;
	}

	void CWidget::HandleEvent(CEvent* event)
    {
		if (event->isHandled && event->stopPropagation)
			return;

		if (event->GetEventType() == CEventType::MouseButtonClick)
		{
			CMouseEvent* mouseEvent = (CMouseEvent*)event;
			fire OnMouseClick(mouseEvent);
		}

		for (CWidget* parent = GetOwner(); parent != nullptr; parent = parent->GetOwner())
		{
			if (event->isHandled && event->stopPropagation)
				return;
			if (parent != this)
				parent->HandleEvent(event);
		}
    }

} // namespace CE::Widgets


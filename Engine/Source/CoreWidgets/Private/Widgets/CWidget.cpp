
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

		this->stylesheet = CreateDefaultSubobject<CSSStyleSheet>("StyleSheet");
	}

	CWidget::~CWidget()
	{
		YGNodeFree(node);
		node = nullptr;
	}

	void CWidget::ShowContextMenu()
	{
		if (contextMenu != nullptr)
			contextMenu->Show();
	}

	void CWidget::HideContextMenu()
	{
		if (contextMenu != nullptr)
			contextMenu->Hide();
	}

	void CWidget::Construct()
	{
		if (GetOwner() != nullptr)
		{
			this->stylesheet->parent = GetOwner()->stylesheet;
		}
	}

	void CWidget::LoadGuiStyleStateProperty(CStylePropertyType property, const CStyleValue& styleValue, GUI::GuiStyleState& outState)
	{
		if (property == CStylePropertyType::Background)
		{
			if (styleValue.IsColor())
				outState.background = styleValue.color;
			else if (styleValue.IsEnum() && styleValue.enumValue.x == CStyleValue::None)
				outState.background = Color(0, 0, 0, 0);
		}
		else if (property == CStylePropertyType::Foreground && styleValue.IsColor())
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
		else if (property == CStylePropertyType::FontSize && styleValue.IsSingle())
		{
			outState.fontSize = (u32)styleValue.single;
		}
		else if (property == CStylePropertyType::FontName && styleValue.IsString())
		{
			outState.fontName = styleValue.string;
		}
	}

	void CWidget::DrawBackground(const GUI::GuiStyleState& styleState)
	{
		auto rect = GetComputedLayoutRect();

		Rect globalRect = GUI::WindowRectToGlobalRect(rect);
		f32 borderWidth = styleState.borderThickness;

		if (styleState.background.a > 0)
		{
			GUI::FillRect(globalRect, styleState.background, styleState.borderRadius);
		}
		if (borderWidth > 0 && styleState.borderColor.a > 0)
		{
			GUI::DrawRect(globalRect + Rect(borderWidth, borderWidth, -borderWidth, -borderWidth), styleState.borderColor, styleState.borderRadius, borderWidth);
		}
	}

	void CWidget::DrawBackground(const GUI::GuiStyleState& styleState, const Rect& rect)
	{
		Rect globalRect = GUI::WindowRectToGlobalRect(rect);
		f32 borderWidth = styleState.borderThickness;

		if (styleState.background.a > 0)
		{
			GUI::FillRect(globalRect, styleState.background, styleState.borderRadius);
		}
		if (borderWidth > 0 && styleState.borderColor.a > 0)
		{
			GUI::DrawRect(globalRect + Rect(borderWidth, borderWidth, -borderWidth, -borderWidth), styleState.borderColor, styleState.borderRadius, borderWidth);
		}
	}

	void CWidget::FillRect(const Color& color, const Rect& localRect, const Vec4& borderRadius)
	{
		Rect globalRect = GUI::WindowRectToGlobalRect(localRect);
		
		if (color.a > 0)
		{
			GUI::FillRect(globalRect, color, borderRadius);
		}
	}

	void CWidget::DrawRect(const Color& color, const Rect& localRect, f32 borderThickness, const Vec4& borderRadius)
	{
		Rect globalRect = GUI::WindowRectToGlobalRect(localRect);
		
		if (color.a > 0)
		{
			GUI::DrawRect(globalRect, color, borderRadius, borderThickness);
		}
	}

	void CWidget::DrawImage(const CTexture& image, const Rect& localRect, const Color& tintColor)
	{

	}

	void CWidget::UpdateStyleIfNeeded()
	{
		if (!needsStyle && !stylesheet->IsDirty())
			return;

		needsStyle = false;

		node->setStyle({}); // Clear style

		computedStyle = {};
		defaultStyleState = {};

		if (GetOwner() != nullptr)
		{
			const auto& parentComputedStyle = GetOwner()->computedStyle;
			const auto& inheritedProperties = CStyle::GetInheritedProperties();
			for (auto property : inheritedProperties)
			{
				if (parentComputedStyle.properties.KeyExists(property))
				{
					computedStyle.properties[property] = parentComputedStyle.properties.Get(property);
				}
			}
		}

		auto selectStyle = stylesheet->SelectStyle(this, stateFlags, subControl);
		computedStyle.ApplyProperties(selectStyle);

		OnBeforeComputeStyle();
		
		for (auto& [property, value] : computedStyle.properties)
		{
			// Non-Yoga properties
			if (value.IsValid()) // Default state
			{
				LoadGuiStyleStateProperty(property, value, defaultStyleState);
			}
			
			if (!value.IsValid())
				continue;
			
			// Yoga Properties
			if (property == CStylePropertyType::Display && value.IsEnum())
			{
				YGNodeStyleSetDisplay(node, (YGDisplay)value.enumValue.x);
			}
			else if (property == CStylePropertyType::Position && value.IsEnum())
			{
				YGNodeStyleSetPositionType(node, (YGPositionType)value.enumValue.x);
			}
			else if (property == CStylePropertyType::Left && value.IsSingle())
			{
				if (value.IsPercentValue())
					YGNodeStyleSetPositionPercent(node, YGEdgeLeft, value.single);
				else
					YGNodeStyleSetPosition(node, YGEdgeLeft, value.single);
			}
			else if (property == CStylePropertyType::Top && value.IsSingle())
			{
				if (value.IsPercentValue())
					YGNodeStyleSetPositionPercent(node, YGEdgeTop, value.single);
				else
					YGNodeStyleSetPosition(node, YGEdgeTop, value.single);
			}
			else if (property == CStylePropertyType::Right && value.IsSingle())
			{
				if (value.IsPercentValue())
					YGNodeStyleSetPositionPercent(node, YGEdgeRight, value.single);
				else
					YGNodeStyleSetPosition(node, YGEdgeRight, value.single);
			}
			else if (property == CStylePropertyType::Bottom && value.IsSingle())
			{
				if (value.IsPercentValue())
					YGNodeStyleSetPositionPercent(node, YGEdgeBottom, value.single);
				else
					YGNodeStyleSetPosition(node, YGEdgeBottom, value.single);
			}
			else if (property == CStylePropertyType::AlignContent && value.IsEnum())
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
						YGNodeStyleSetPaddingPercent(node, YGEdgeAll, value.single);
					else
						YGNodeStyleSetPadding(node, YGEdgeAll, value.single);
				}
				else if (value.IsVector())
				{
					// Left
					if (value.enumValue.x == CStyleValue::Percent)
						YGNodeStyleSetPaddingPercent(node, YGEdgeLeft, value.vector.x);
					else
						YGNodeStyleSetPadding(node, YGEdgeLeft, value.vector.x);
					// Top
					if (value.enumValue.y == CStyleValue::Percent)
						YGNodeStyleSetPaddingPercent(node, YGEdgeTop, value.vector.y);
					else
						YGNodeStyleSetPadding(node, YGEdgeTop, value.vector.y);
					// Right
					if (value.enumValue.z == CStyleValue::Percent)
						YGNodeStyleSetPaddingPercent(node, YGEdgeRight, value.vector.z);
					else
						YGNodeStyleSetPadding(node, YGEdgeRight, value.vector.z);
					// Bottom
					if (value.enumValue.w == CStyleValue::Percent)
						YGNodeStyleSetPaddingPercent(node, YGEdgeBottom, value.vector.w);
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

		OnAfterComputeStyle();

		needsStyle = false;
		if (!IsMenu())
		{
			SetNeedsLayout();
		}

		stylesheet->SetDirty(false);

		for (auto menu : attachedMenus)
		{
			menu->SetNeedsStyle();
			menu->UpdateStyleIfNeeded();
		}

		if (IsContainer())
		{
			for (CWidget* subWidget : attachedWidgets)
			{
				if (subWidget == nullptr || subWidget == this)
					continue;
				subWidget->needsStyle = true;
				subWidget->UpdateStyleIfNeeded();
			}
		}
	}

	inline bool CWidget::NeedsLayout()
	{
		for (auto child : attachedWidgets)
		{
			if (child->NeedsLayout())
				return true;
		}

		for (auto menu : attachedMenus)
		{
			if (menu->IsShown() && menu->NeedsLayout())
				return true;
		}

		return needsLayout;
	}

	bool CWidget::NeedsStyle()
	{
		return needsStyle;
	}

	void CWidget::SetNeedsLayout(bool set)
	{
		for (auto child : attachedWidgets)
		{
			child->SetNeedsLayout(set);
		}

		for (auto menu : attachedMenus)
		{
			menu->SetNeedsLayout(set);
		}

		needsLayout = set;
	}

	void CWidget::SetNeedsStyle(bool set)
	{
		for (auto child : attachedWidgets)
		{
			child->SetNeedsStyle(set);
		}

		needsStyle = set;
	}

	void CWidget::UpdateLayoutIfNeeded()
	{
		if (!NeedsLayout())
			return;

		for (auto menu : attachedMenus)
		{
			if (menu->IsShown())
				menu->UpdateLayoutIfNeeded();
		}
        
		if (IsMenuBar())
		{
			CMenuBar* menuBar = (CMenuBar*)this;
			CWidget* parent = menuBar->GetOwner();
			CWindow* parentWindow = nullptr;

			// Only calculate layout of menu bar if it is owned by a Dock Space window which doesn't have it's own layout
			if (parent != nullptr && parent->IsWindow() && (parentWindow = (CWindow*)parent)->IsDockSpaceWindow())
			{
				void* windowHandle = parentWindow->GetPlatformHandle();
				auto size = PlatformApplication::Get()->GetWindowSize(windowHandle);
				
				YGNodeCalculateLayout(node, size.x, size.y, YGDirectionLTR);

				SetNeedsLayout(false);
			}
		}
		else if (IsWindow())
        {
			CWindow* window = (CWindow*)this;
			if (!window->IsDockSpaceWindow() || RequiresIndependentLayoutCalculation())
			{
				SetNeedsStyle(); // Force update style
				UpdateStyleIfNeeded();

				Vec2 size = CalculateIntrinsicContentSize(YGUndefined, YGUndefined);
				if (size.x <= 0) size.x = YGUndefined;
				if (size.y <= 0) size.y = YGUndefined;
				YGNodeCalculateLayout(node, size.x, size.y, YGDirectionLTR);

				SetNeedsLayout(false);
			}
			else
			{
				if (window->GetMenuBar() != nullptr)
				{
					window->GetMenuBar()->UpdateLayoutIfNeeded();
				}
			}

			needsLayout = false;
        }
		else if ((IsContainer() && GetOwner() == nullptr) || RequiresIndependentLayoutCalculation())
		{
			SetNeedsStyle(); // Force update style
			UpdateStyleIfNeeded();

			Vec2 size = CalculateIntrinsicContentSize(YGUndefined, YGUndefined);
			if (size.x <= 0) size.x = YGUndefined;
			if (size.y <= 0) size.y = YGUndefined;
			if (GetOwner() != nullptr)
			{
				auto ownerSize = GetOwner()->GetComputedLayoutSize();
				auto ownerPadding = GetOwner()->GetComputedLayoutPadding();
				size.x = ownerSize.x;
				//size.y = ownerSize.y;
			}

			YGNodeCalculateLayout(node, size.x, size.y, YGDirectionLTR);

			SetNeedsLayout(false);

			needsLayout = false;
		}

		for (auto child : attachedWidgets)
		{
			child->UpdateLayoutIfNeeded();
		}
	}

	void CWidget::OnAttachedTo(CWidget* parent)
	{
		if (inheritedPropertiesInitialized) // Inherited properties can only be initialized once
			return;

		SetNeedsStyle();
		SetNeedsLayout();

		inheritedPropertiesInitialized = true;
	}

	void CWidget::OnDetachedFrom(CWidget* parent)
	{

	}

	void CWidget::OnSubobjectAttached(Object* subobject)
	{
		if (subobject == nullptr || subobject == this) // Impossible edge cases
			return;
		
		if (subobject->GetClass()->IsSubclassOf<CMenu>() && !subobject->GetClass()->IsSubclassOf<CMenuBar>())
		{
			CMenu* menu = (CMenu*)subobject;
			attachedMenus.Add(menu);
			menu->ownerWindow = GetOwnerWindow();
			menu->parent = this;
		}
		else if (IsContainer() && subobject->GetClass()->IsSubclassOf<CWidget>() && IsSubWidgetAllowed(subobject->GetClass()))
		{
			CWidget* subWidget = (CWidget*)subobject;
			attachedWidgets.Add(subWidget);
			OnSubWidgetAttached(subWidget);
			subWidget->parent = this;
			if (IsWindow())
				subWidget->ownerWindow = (CWindow*)this;
			else
				subWidget->ownerWindow = GetOwnerWindow();
			subWidget->OnAttachedTo(this);

			if (!subWidget->RequiresIndependentLayoutCalculation())
			{
				YGNodeSetMeasureFunc(node, nullptr);

				auto childCount = YGNodeGetChildCount(node);
				YGNodeInsertChild(node, subWidget->node, childCount);
			}
		}
	}

	void CWidget::OnSubobjectDetached(Object* subobject)
	{
		if (subobject == nullptr)
			return;

		if (subobject->GetClass()->IsSubclassOf<CMenu>())
		{
			CMenu* menu = (CMenu*)subobject;
			attachedMenus.Remove(menu);
			menu->ownerWindow = nullptr;
			menu->parent = nullptr;

			if (subobject->GetClass()->IsSubclassOf<CContextMenu>() && contextMenu != nullptr && contextMenu == (CContextMenu*)subobject)
			{
				contextMenu->parent = nullptr;
				contextMenu->ownerWindow = nullptr;
				contextMenu = nullptr;
			}
		}
		else if (subobject->GetClass()->IsSubclassOf<CWidget>())
		{
			CWidget* subWidget = (CWidget*)subobject;
			attachedWidgets.Remove(subWidget);
			OnSubWidgetDetached(subWidget);
			subWidget->OnDetachedFrom(this);
			subWidget->parent = nullptr;

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

	void CWidget::PollBasicMouseEvents(bool hovered, bool leftMouseHeld, CStateFlag& stateFlags)
	{
		if (isHovered != hovered)
		{
			isHovered = hovered;
			SetNeedsStyle();

			if (isHovered) // Mouse Enter
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
			else // Mouse Leave
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
		}

		if (isLeftMousePressedInside != (isHovered && leftMouseHeld))
		{
			isLeftMousePressedInside = isHovered && leftMouseHeld;
			SetNeedsStyle();

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
			else if (isHovered && !leftMouseHeld) // Release inside
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
			else if (!leftMouseHeld) // Release outside
			{
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
		}

		if (isLeftMousePressedInside)
		{
			if (EnumHasAnyFlags(stateFlags, CStateFlag::Hovered) || !EnumHasAnyFlags(stateFlags, CStateFlag::Pressed))
				SetNeedsStyle();

			EnumAddFlags(stateFlags, CStateFlag::Pressed);
			EnumRemoveFlags(stateFlags, CStateFlag::Hovered);
		}
		else if (isHovered)
		{
			if (EnumHasAnyFlags(stateFlags, CStateFlag::Pressed) || !EnumHasAnyFlags(stateFlags, CStateFlag::Hovered))
				SetNeedsStyle();

			EnumAddFlags(stateFlags, CStateFlag::Hovered);
			EnumRemoveFlags(stateFlags, CStateFlag::Pressed);
		}
		else
		{
			if (EnumHasAnyFlags(stateFlags, CStateFlag::Hovered | CStateFlag::Pressed))
				SetNeedsStyle();

			EnumRemoveFlags(stateFlags, CStateFlag::Hovered);
			EnumRemoveFlags(stateFlags, CStateFlag::Pressed);
		}
	}

	void CWidget::ClearChildNodes()
	{
		YGNodeRemoveAllChildren(node);
		YGNodeSetMeasureFunc(node, MeasureFunctionCallback);
	}

	void CWidget::ReAddChildNodes()
	{
		ClearChildNodes();

		YGNodeSetMeasureFunc(node, nullptr);

		for (auto subWidget : attachedWidgets)
		{
			if (!subWidget->RequiresIndependentLayoutCalculation())
			{
				auto childCount = YGNodeGetChildCount(node);
				YGNodeInsertChild(node, subWidget->node, childCount);
			}
		}
	}

	void CWidget::Render()
	{
		if (IsVisible())
		{
			auto rect = GetComputedLayoutRect();

			if (!isnan(rect.x) && !isnan(rect.y))
			{
				screenPos = GUI::WindowRectToGlobalRect(rect).min;
			}

			CFontManager::Get().PushFont(defaultStyleState.fontSize, defaultStyleState.fontName);
			OnDrawGUI();
			CFontManager::Get().PopFont();
		}

		for (auto menu : attachedMenus)
		{
			if (menu->IsShown())
				menu->Render();
		}

		UpdateStyleIfNeeded();
		UpdateLayoutIfNeeded();
	}

	bool CWidget::TestFocus()
	{
		CWindow* thisWindow = nullptr;
		if (IsWindow())
		{
			thisWindow = (CWindow*)this;
			return GUI::IsWindowFocused(thisWindow->GetTitle(), GUI::FOCUS_ChildWindows | GUI::FOCUS_DockHierarchy);
		}

		return GUI::IsItemFocused();
	}

	void CWidget::PollEvents()
	{
		bool focused = TestFocus();

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

		if (!IsInteractable())
			return;

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

				stateFlags |= CStateFlag::Hovered;
				SetNeedsStyle();
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
                
                EnumRemoveFlags(stateFlags, CStateFlag::Hovered);
				SetNeedsStyle();
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
			CWindow* thisWindow = (CWindow*)this;

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

		if (IsLeftMouseHeld() && IsHovered())
		{
            if (EnumHasAnyFlags(stateFlags, CStateFlag::Hovered) || !EnumHasAnyFlags(stateFlags, CStateFlag::Pressed))
			{
				SetNeedsStyle();
				SetNeedsLayout();
			}
			EnumAddFlags(stateFlags, CStateFlag::Pressed);
			EnumRemoveFlags(stateFlags, CStateFlag::Hovered);
		}
		else if (IsHovered())
		{
            if (!EnumHasAnyFlags(stateFlags, CStateFlag::Hovered) || EnumHasAnyFlags(stateFlags, CStateFlag::Pressed))
			{
				SetNeedsStyle();
				SetNeedsLayout();
			}
			EnumAddFlags(stateFlags, CStateFlag::Hovered);
			EnumRemoveFlags(stateFlags, CStateFlag::Pressed);
		}
		else
		{
            if (EnumHasAnyFlags(stateFlags, CStateFlag::Hovered | CStateFlag::Pressed))
			{
				SetNeedsStyle();
				SetNeedsLayout();
			}
			EnumRemoveFlags(stateFlags, CStateFlag::Pressed | CStateFlag::Hovered);
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

	void CWidget::SetStyleSheet(const String& stylesheetText)
	{
		this->stylesheetText = stylesheetText;

		CSSStyleSheet* parentStylesheet = nullptr;
		auto owner = GetOwner();

		while (owner != nullptr)
		{
			if (owner->stylesheet != nullptr)
			{
				parentStylesheet = (CSSStyleSheet*)owner->stylesheet;
				break;
			}
			owner = owner->GetOwner();
		}

		if (parentStylesheet == nullptr)
		{
			parentStylesheet = (CSSStyleSheet*)GetStyleManager()->globalStyleSheet;
		}

		CSSParser::ParseStyleSheet((CSSStyleSheet*)stylesheet, stylesheetText);
		stylesheet->parent = parentStylesheet;
	}

	const String& CWidget::GetStyleSheet() const
	{
		return stylesheetText;
	}

	void CWidget::LoadStyleSheet(const Name& resourcePath)
	{
		GetStyleManager()->LoadStyleSheet(resourcePath, this->stylesheet);
	}

	CWidget* CWidget::GetOwner()
	{
		return parent;
	}

	CWindow* CWidget::GetOwnerWindow()
	{
		if (ownerWindow != nullptr)
			return ownerWindow;

		auto owner = GetOwner();
		while (owner != nullptr)
		{
			if (owner->IsWindow())
			{
				this->ownerWindow = (CWindow*)owner;
				return ownerWindow;
			}
			owner = owner->GetOwner();
		}
		return ownerWindow;
	}

	int CWidget::GetSubWidgetIndex(CWidget* subWidget)
	{
		for (int i = 0; i < attachedWidgets.GetSize(); i++)
		{
			if (attachedWidgets[i] == subWidget)
				return i;
		}
		return -1;
	}

	int CWidget::GetSubWidgetCount()
	{
		return attachedWidgets.GetSize();
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

	CWidget* CWidget::GetSubWidgetAt(int index)
	{
		if (index < 0 || index >= attachedWidgets.GetSize())
			return nullptr;
		return attachedWidgets[index];
	}

	bool CWidget::IsSubWidgetAllowed(ClassType* subWidgetClass)
	{
		return subWidgetClass->IsSubclassOf<CWidget>();
	}
	

	void CWidget::HandleEvent(CEvent* event)
    {
		if (event->isHandled && event->stopPropagation)
			return;

		if (!event->isHandled && event->GetEventType() == CEventType::MouseButtonClick)
		{
			CMouseEvent* mouseEvent = (CMouseEvent*)event;
			if (mouseEvent->mouseButton == 0) // Left click
			{
				emit OnMouseClick(mouseEvent);
				event->MarkHandled();
			}
			else if (mouseEvent->mouseButton == 1) // Right click
			{
				if (contextMenu != nullptr)
				{
					contextMenu->ShowContextMenu();
				}

				emit OnMouseRightClick(mouseEvent);
				event->MarkHandled();
			}
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


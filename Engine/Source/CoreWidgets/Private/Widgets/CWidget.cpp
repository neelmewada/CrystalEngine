
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

		detachedNode = YGNodeNew();
		YGNodeSetContext(detachedNode, this);
		YGNodeSetMeasureFunc(detachedNode, MeasureFunctionCallback);

		this->stylesheet = CreateDefaultSubobject<CSSStyleSheet>("StyleSheet");
	}

	CWidget::~CWidget()
	{
		YGNodeFree(node);
		node = nullptr;
		YGNodeFree(detachedNode);
		detachedNode = nullptr;
	}

	void CWidget::OnBeforeDestroy()
	{
		Super::OnBeforeDestroy();

		if (GetWidgetDebugger() != nullptr && GetWidgetDebugger()->debugWidget == this)
		{
			GetWidgetDebugger()->SetDebugWidget(nullptr);
		}
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

	void CWidget::LoadGuiStyleState(const CStyle& from, GUI::GuiStyleState& outState)
	{
		for (const auto& [property, value] : from.properties)
		{
			LoadGuiStyleStateProperty(property, value, outState);
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
		else if (property == CStylePropertyType::ShadowColor && styleValue.IsColor())
		{
			outState.shadowColor = styleValue.color;
		}
		else if (property == CStylePropertyType::ShadowOffset && styleValue.IsVector())
		{
			outState.shadowOffset = Vec2(styleValue.vector.x, styleValue.vector.y);
		}
		else if (property == CStylePropertyType::Cursor && styleValue.IsEnum())
		{
			CCursor cursorValue = (CCursor)styleValue.enumValue.x;
			if (cursorValue == CCursor::Arrow)
				outState.cursor = GUI::MouseCursor_Arrow;
			else if (cursorValue == CCursor::Hand)
				outState.cursor = GUI::MouseCursor_Hand;
		}
	}

	void CWidget::DrawShadow(const GUI::GuiStyleState& styleState)
	{
		bool hasShadow = styleState.shadowColor.a > 0 && (styleState.shadowOffset.x != 0 || styleState.shadowOffset.y != 0);
		if (!hasShadow)
			return;

		auto localRect = GetComputedLayoutRect();
		Rect globalRect = GUI::WidgetSpaceToScreenSpace(localRect);

		const auto& shadowOffset = styleState.shadowOffset;
		GUI::FillRect(globalRect + Rect(shadowOffset.x, shadowOffset.y, shadowOffset.x, shadowOffset.y), styleState.shadowColor, styleState.borderRadius);
	}

	void CWidget::DrawShadow(const GUI::GuiStyleState& styleState, const Rect& localRect)
	{
		bool hasShadow = styleState.shadowColor.a > 0 && (styleState.shadowOffset.x != 0 || styleState.shadowOffset.y != 0);
		if (!hasShadow)
			return;

		Rect globalRect = GUI::WidgetSpaceToScreenSpace(localRect);

		const auto& shadowOffset = styleState.shadowOffset;
		GUI::FillRect(globalRect + Rect(shadowOffset.x, shadowOffset.y, shadowOffset.x, shadowOffset.y), styleState.shadowColor, styleState.borderRadius);
	}

	void CWidget::DrawBackground(const GUI::GuiStyleState& styleState, bool allowDebug)
	{
		DrawBackground(styleState, GetComputedLayoutRect(), allowDebug);
	}

	void CWidget::DrawBackground(const GUI::GuiStyleState& styleState, const Rect& rect, bool allowDebug)
	{
		Rect globalRect = GUI::WidgetSpaceToScreenSpace(rect);
		f32 borderWidth = styleState.borderThickness;
		bool hasShadow = styleState.shadowColor.a > 0 && (styleState.shadowOffset.x != 0 || styleState.shadowOffset.y != 0);

		if (hasShadow)
		{
			const auto& shadowOffset = styleState.shadowOffset;
			GUI::FillRect(globalRect + Rect(shadowOffset.x, shadowOffset.y, shadowOffset.x, shadowOffset.y), styleState.shadowColor, styleState.borderRadius);
		}
		if (styleState.background.a > 0)
		{
			GUI::FillRect(globalRect, styleState.background, styleState.borderRadius);
		}
		if (borderWidth > 0 && styleState.borderColor.a > 0)
		{
			Rect globalBorderRect = GUI::WidgetSpaceToScreenSpace(GetComputedBorderRect());
			auto borderWidths = GetComputedLayoutBorder();
			globalBorderRect += Rect(borderWidths.left / 2, borderWidths.top / 2, -borderWidths.right / 2, -borderWidths.bottom / 2);
			GUI::DrawRect(globalBorderRect, styleState.borderColor, styleState.borderRadius, borderWidth);
		}

		if (allowDebug && forceDebugDrawMode != CDebugBackgroundFilter::None)
		{
			DrawDebugBackground(forceDebugDrawMode);
		}
		else if (allowDebug && debugDraw)
		{
			DrawDebugBackground(filter);
		}
	}

	void CWidget::DrawDebugBackground(CDebugBackgroundFilter filter)
	{
		Rect globalMargin = GUI::WidgetSpaceToScreenSpace(GetComputedMarginRect());
		Rect margin = GetComputedLayoutMargin();
		Rect globalBorder = GUI::WidgetSpaceToScreenSpace(GetComputedBorderRect());
		Rect border = GetComputedLayoutBorder();
		Rect globalPadding = GUI::WidgetSpaceToScreenSpace(GetComputedPaddingRect());
		Rect padding = GetComputedLayoutPadding();
		Rect globalRect = GUI::WidgetSpaceToScreenSpace(GetComputedIntrinsicSizeRect());
		
		if (EnumHasFlag(filter, CDebugBackgroundFilter::Margin))
		{
			GUI::FillRectWidth(globalMargin, globalMargin + Rect(margin.min, -margin.max), Color::FromRGBA32(173, 128, 82));
		}
		if (EnumHasFlag(filter, CDebugBackgroundFilter::Border))
		{
			GUI::FillRectWidth(globalBorder, globalBorder + Rect(border.min, -border.max), Color::FromRGBA32(227, 195, 129));
		}
		if (EnumHasFlag(filter, CDebugBackgroundFilter::Padding))
		{
			GUI::FillRectWidth(globalPadding, globalPadding + Rect(padding.min, -padding.max), Color::FromRGBA32(183, 196, 127));
		}
		if (EnumHasFlag(filter, CDebugBackgroundFilter::IntrinsicSize))
		{
			GUI::FillRect(globalRect, Color::FromRGBA32(135, 178, 188));
		}
	}

	void CWidget::FillRect(const Color& color, const Rect& localRect, const Vec4& borderRadius)
	{
		Rect globalRect = GUI::WidgetSpaceToScreenSpace(localRect);
		
		if (color.a > 0)
		{
			GUI::FillRect(globalRect, color, borderRadius);
		}
	}

	void CWidget::DrawRect(const Color& color, const Rect& localRect, f32 borderThickness, const Vec4& borderRadius)
	{
		Rect globalRect = GUI::WidgetSpaceToScreenSpace(localRect);
		
		if (color.a > 0)
		{
			GUI::DrawRect(globalRect, color, borderRadius, borderThickness);
		}
	}

	void CWidget::DrawImage(const CTexture& image, const Rect& localRect, const Color& tintColor)
	{
		if (image.IsValid())
		{
			GUI::Image(localRect, image.id, defaultStyleState);
		}
	}

	void CWidget::FillCircle(const Color& color, const Rect& localRect)
	{
		Rect globalRect = GUI::WidgetSpaceToScreenSpace(localRect);

		GUI::FillCircle(globalRect, color);
	}

	void CWidget::DrawCircle(const Color& color, const Rect& localRect, f32 borderThickness)
	{
		Rect globalRect = GUI::WidgetSpaceToScreenSpace(localRect);

		GUI::DrawCircle(globalRect, color, borderThickness);
	}

	void CWidget::SetEnabled(bool enabled)
	{
		if (isDisabled != !enabled)
		{
			isDisabled = !enabled;
			if (isDisabled)
			{
				stateFlags = CStateFlag::Disabled;
			}
			else
			{
				EnumRemoveFlags(stateFlags, CStateFlag::Disabled);
			}

			SetNeedsStyle();
		}
	}

	bool CWidget::IsDebugModeEnabled()
	{
		if (GetWidgetDebugger() != nullptr && GetWidgetDebugger()->enableLayoutDebugMode)
			return true;

		if (IsWindow())
			return ((CWindow*)this)->IsWindowDebugMode();
		auto window = GetOwnerWindow();
		if (window == nullptr)
			return false;
		return window->IsWindowDebugMode();
	}

	void CWidget::OnAttachedTo(CWidget* parent)
	{
		SetNeedsStyleRecursive();
		SetNeedsLayout();
	}

	void CWidget::OnDetachedFrom(CWidget* parent)
	{

	}

	void CWidget::OnSubobjectAttached(Object* subobject)
	{
		if (subobject == nullptr || subobject == this) // Impossible edge cases
			return;
		
		if (subobject->IsOfType<CMenu>() && !subobject->IsOfType<CMenuBar>())
		{
			CMenu* menu = (CMenu*)subobject;
			attachedMenus.Add(menu);
			menu->ownerWindow = GetOwnerWindow();
			menu->parent = this;
		}
		else if (subobject->IsOfType<CPopup>())
		{
			// Do not add popups to attachedWidgets
		}
		else if (IsContainer() && subobject->IsOfType<CWidget>() && IsSubWidgetAllowed(subobject->GetClass()))
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

			if (IsOfType<CToolBar>())
			{
				subWidget->AddStyleClass("ToolBarItem");
			}

			if (!subWidget->IsLayoutCalculationRoot())
			{
				YGNodeSetMeasureFunc(node, nullptr);

				auto childCount = YGNodeGetChildCount(node);
				YGNodeInsertChild(node, subWidget->node, childCount);
			}
			else // SubWidget requires independent layout calculation. Ex: Child window, Splitter view, etc. Insert empty (detached) node for that.
			{
				YGNodeSetMeasureFunc(node, nullptr);

				auto childCount = YGNodeGetChildCount(node);
				YGNodeInsertChild(node, subWidget->detachedNode, childCount);
			}
		}
		else if (subobject->IsOfType<CWidget>())
		{
			CWidget* widget = (CWidget*)subobject;
			if (widget != this)
			{
				widget->parent = this;
			}
		}
	}

	void CWidget::OnSubobjectDetached(Object* subobject)
	{
		if (subobject == nullptr)
			return;

		if (subobject->IsOfType<CMenu>())
		{
			CMenu* menu = (CMenu*)subobject;
			attachedMenus.Remove(menu);
			menu->ownerWindow = nullptr;
			menu->parent = nullptr;

			if (subobject->IsOfType<CContextMenu>() && contextMenu != nullptr && contextMenu == (CContextMenu*)subobject)
			{
				contextMenu->parent = nullptr;
				contextMenu->ownerWindow = nullptr;
				contextMenu = nullptr;
			}
		}
		else if (subobject->IsOfType<CWidget>())
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

	void CWidget::ClearChildNodes()
	{
		YGNodeRemoveAllChildren(node);
		YGNodeSetMeasureFunc(node, MeasureFunctionCallback);
	}

	void CWidget::ReAddChildNodes()
	{
		ClearChildNodes();

		if (attachedWidgets.NonEmpty())
			YGNodeSetMeasureFunc(node, nullptr);

		for (auto subWidget : attachedWidgets)
		{
			if (!subWidget->IsLayoutCalculationRoot())
			{
				auto childCount = YGNodeGetChildCount(node);
				YGNodeInsertChild(node, subWidget->node, childCount);
			}
		}
	}

	void CWidget::Render()
	{
		UpdateStyleIfNeeded();
		UpdateLayoutIfNeeded();

		if (IsVisible())
		{
			auto rect = GetComputedLayoutRect();

			if (ShouldHandleBackgroundDraw())
				DrawDefaultBackground();

			if (IsDebugModeEnabled())
			{
				if (debugId == 0) debugId = GenerateRandomU32();
				
				bool hovered = false, held = false;
				if (GUI::InvisibleButton(rect, debugId, hovered, held) && GetWidgetDebugger() != nullptr)
				{
					GetWidgetDebugger()->SetDebugWidget(this);
				}

				if (hovered)
				{
					DrawDebugBackground();
				}
			}

			if (!isnan(rect.x) && !isnan(rect.y))
			{
				screenPos = GUI::WidgetSpaceToScreenSpace(rect).min;
			}
			
			if (IsDisabled()) GUI::BeginDisabled();

			if (isHovered && defaultStyleState.cursor != GUI::MouseCursor_None)
			{
				GUI::SetMouseCursor(defaultStyleState.cursor);
			}

			CFontManager::Get().PushFont(defaultStyleState.fontSize, defaultStyleState.fontName);
			OnDrawGUI();
			CFontManager::Get().PopFont();

			if (IsDisabled()) GUI::EndDisabled();

			// Always calculate layout twice if it's first draw call (on next draw).
			// So internal state can get updated children sizes & positions from ImGui, especially the window sizes.
			if (firstDraw)
			{
				firstDraw = false;

				SetNeedsStyleRecursive();
				SetNeedsLayoutRecursive();
			}
		}
		else
		{
			auto rect = GetComputedLayoutRect();

			if (IsDebugModeEnabled())
			{
				if (debugId == 0) debugId = GenerateRandomU32();

				bool hovered = false, held = false;
				GUI::InvisibleButton(rect, debugId, hovered, held);

				if (hovered)
				{
					DrawDebugBackground();
				}
			}
		}

		for (auto menu : attachedMenus)
		{
			if (menu->IsShown())
				menu->Render();
		}
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
		if (IsDisabled())
			return;

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
		else // A Window
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
			if (EnumHasAnyFlags(stateFlags, CStateFlag::Hovered | CStateFlag::Pressed))
			{
				SetNeedsStyle();
			}

			EnumAddFlags(stateFlags, CStateFlag::Pressed);
			EnumRemoveFlags(stateFlags, CStateFlag::Hovered);
		}
		else if (IsHovered())
		{
			if (!EnumHasAnyFlags(stateFlags, CStateFlag::Hovered) || EnumHasAnyFlags(stateFlags, CStateFlag::Pressed))
			{
				SetNeedsStyle();
			}

			EnumAddFlags(stateFlags, CStateFlag::Hovered);
			EnumRemoveFlags(stateFlags, CStateFlag::Pressed);
		}
		else
		{
			if (EnumHasAnyFlags(stateFlags, CStateFlag::Hovered | CStateFlag::Pressed))
			{
				SetNeedsStyle();
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

	bool CWidget::IsLayoutCalculationRoot()
	{
		if (independentLayout)
		{
			return true;
		}

		return IsWindow();
	}

	bool CWidget::NeedsLayout(bool recursive)
	{
		if (!recursive)
			return needsLayout;

		for (auto widget : attachedWidgets)
		{
			if (!widget->IsWindow() && widget->NeedsLayout(recursive))
				return true;
		}

		return needsLayout;
	}

	bool CWidget::NeedsStyle(bool recursive)
	{
		if (!recursive)
			return needsStyle;

		for (auto widget : attachedWidgets)
		{
			if (widget->NeedsStyle(recursive))
				return true;
		}

		return needsStyle;
	}

	bool CWidget::ChildrenNeedsLayout()
	{
		if (needsLayout)
			return true;

		for (auto widget : attachedWidgets)
		{
			if (widget->NeedsLayout(false))
				return true;
		}

		return false;
	}

	bool CWidget::ChildrenNeedsStyle()
	{
		if (needsStyle)
			return true;

		for (auto widget : attachedWidgets)
		{
			if (widget->NeedsStyle(false))
				return true;
		}

		return false;
	}

	void CWidget::SetNeedsLayout(bool set, bool recursive)
	{
		needsLayout = set;
		if (set && YGNodeGetChildCount(node) == 0)
		{
			if (!YGNodeHasMeasureFunc(node))
			{
				YGNodeSetMeasureFunc(node, MeasureFunctionCallback);
			}

			YGNodeMarkDirty(node);
		}

		if (!recursive)
			return;

		for (auto widget : attachedWidgets)
		{
			if (set || !widget->IsLayoutCalculationRoot())
				widget->SetNeedsLayout(set, recursive);
		}
	}

	void CWidget::SetNeedsStyle(bool set, bool recursive)
	{
		needsStyle = set;

		if (!recursive)
			return;

		for (auto widget : attachedWidgets)
		{
			widget->SetNeedsStyle(set, recursive);
		}
	}

	void CWidget::UpdateLayoutIfNeeded()
	{
		if (!NeedsLayoutRecursive())
			return;
		if (!IsLayoutCalculationRoot())
			return;

		UpdateStyleIfNeeded();

		auto parent = GetOwner();
		Vec2 parentSize = Vec2(YGUndefined, YGUndefined);

		if (IsWindow())
		{
			CWindow* window = (CWindow*)this;
			auto winSize = window->GetWindowSize();
			if (winSize.width > 0 && !IsNan(winSize.width))
				parentSize.width = winSize.width;
			if (winSize.height > 0 && !IsNan(winSize.height))
				parentSize.height = winSize.height;
		}
		else if ((!IsNan(sizeConstraint.x) && sizeConstraint.x > 0) || (!IsNan(sizeConstraint.y) && sizeConstraint.y > 0))
		{
			if (!IsNan(sizeConstraint.x))
				parentSize.width = sizeConstraint.x;
			if (!IsNan(sizeConstraint.y))
				parentSize.height = sizeConstraint.y;
		}
		else if (parent != nullptr)
		{
			parent->UpdateLayoutIfNeeded();
			parentSize = parent->GetComputedLayoutSize();
		}
		
		YGNodeCalculateLayout(node, parentSize.width, parentSize.height, YGDirectionLTR);

		SetNeedsLayoutRecursive(false);
		needsLayout = false;
	}

	void CWidget::UpdateStyleIfNeeded()
	{
		if (!NeedsStyle())
			return;

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

		for (const auto& [property, value] : selectStyle.properties)
		{
			// Non-Yoga properties
			if (value.IsValid())
			{
				LoadGuiStyleStateProperty(property, value, defaultStyleState); // Default state
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

		for (auto widget : attachedWidgets)
		{
			widget->UpdateStyleIfNeeded();
		}

		for (auto menu : attachedMenus)
		{
			menu->UpdateStyleIfNeeded();
		}
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

	bool CWidget::IsWidgetPresentInParentHierarchy(CWidget* widget)
	{
		auto owner = GetOwner();
		while (owner != nullptr)
		{
			if (owner == widget)
				return true;
			owner = owner->GetOwner();
		}
		return false;
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

    void CWidget::AddSubWidget(CWidget* subWidget)
    {
		AttachSubobject(subWidget);

		SetNeedsStyle();
		SetNeedsLayout();
    }

    void CWidget::RemoveSubWidget(CWidget* subWidget)
    {
		DetachSubobject(subWidget);

		SetNeedsStyle();
		SetNeedsLayout();
    }

	Array<CWidget*> CWidget::RemoveAllSubWidgets()
	{
		Array<CWidget*> result{};

		for (int i = attachedWidgets.GetSize() - 1; i >= 0; i--)
		{
			result.Add(attachedWidgets[i]);
			DetachSubobject(attachedWidgets[i]);
		}
		attachedWidgets.Clear();

		SetNeedsStyle();
		SetNeedsLayout();

		return result;
	}

	void CWidget::DestroyAllSubWidgets()
	{
		for (int i = attachedWidgets.GetSize() - 1; i >= 0; i--)
		{
			auto widget = attachedWidgets[i];
			DetachSubobject(widget);
			widget->Destroy();
		}
		attachedWidgets.Clear();
		
		SetNeedsStyle();
		SetNeedsLayout();
	}

	bool CWidget::IsSubWidgetAllowed(ClassType* subWidgetClass)
	{
		return subWidgetClass->IsSubclassOf<CWidget>();
	}

	CStyleValue CWidget::GetComputedPropertyValue(CStylePropertyType property)
	{
		if (computedStyle.properties.KeyExists(property))
			return computedStyle.properties[property];
		switch (property)
		{
		case CStylePropertyType::FontName:
			return CStyleValue(CFontManager::Get().GetCurrentFont().fontName.GetString());
		case CStylePropertyType::FontSize:
			return CStyleValue((f32)CFontManager::Get().GetCurrentFont().pointSize);
		}
		return CStyleValue();
	}

	void CWidget::HandleEvent(CEvent* event)
    {
		if (event->isHandled && event->stopPropagation)
			return;

		GUI::MouseCursor mouseCursor = GUI::MouseCursor_None;

		if (event->GetEventType() == CEventType::MouseEnter || event->GetEventType() == CEventType::MouseMove)
		{
			if (computedStyle.properties.KeyExists(CStylePropertyType::Cursor))
			{
				CCursor cursor = (CCursor)computedStyle.properties[CStylePropertyType::Cursor].enumValue.x;
				switch (cursor)
				{
				case CCursor::Arrow:
					mouseCursor = GUI::MouseCursor_Arrow;
					GUI::SetMouseCursor(mouseCursor);
					break;
				case CCursor::Hand:
					mouseCursor = GUI::MouseCursor_Hand;
					GUI::SetMouseCursor(mouseCursor);
					break;
				}
			}
		}
		else if (event->GetEventType() == CEventType::MouseLeave)
		{
			if (computedStyle.properties.KeyExists(CStylePropertyType::Cursor))
			{
				CCursor cursor = (CCursor)computedStyle.properties[CStylePropertyType::Cursor].enumValue.x;
				if (cursor != CCursor::Inherited)
				{
					mouseCursor = GUI::MouseCursor_Arrow;
					GUI::SetMouseCursor(mouseCursor);
				}
			}
		}

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


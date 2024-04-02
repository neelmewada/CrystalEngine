#include "CrystalWidgets.h"

namespace CE::Widgets
{
	YGSize CWidget::MeasureFunctionCallback(YGNodeConstRef nodeRef,
		float width,
		YGMeasureMode widthMode,
		float height,
		YGMeasureMode heightMode)
	{
		CWidget* widget = (CWidget*)YGNodeGetContext(nodeRef);
		if (widget == nullptr)
			return YGSize();

		auto size = widget->CalculateIntrinsicSize(width, height);
		return YGSize{ size.x, size.y };
	}

	CWidget::CWidget()
	{
		node = YGNodeNew();
		YGNodeSetContext(node, this);
		YGNodeSetMeasureFunc(node, MeasureFunctionCallback);

		detachedNode = YGNodeNew();
		YGNodeSetContext(detachedNode, this);
		YGNodeSetMeasureFunc(detachedNode, MeasureFunctionCallback);

		this->styleSheet = CreateDefaultSubobject<CSSStyleSheet>("StyleSheet");
	}

	CWidget::~CWidget()
	{
		YGNodeFree(node);
		node = nullptr;

		YGNodeFree(detachedNode);
		detachedNode = nullptr;
	}

	void CWidget::OnAfterConstruct()
	{
		if (!IsDefaultInstance())
		{
			Construct();
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
			if (!subWidget->IsEnabled())
				continue;

			if (subWidget->IsLayoutCalculationRoot())
			{
				// SubWidget requires independent layout calculation for its children.
				// Ex: Child window, Splitter view, etc. Insert a new root (detached) node for that.
				YGNodeSetMeasureFunc(node, nullptr);

				auto childCount = YGNodeGetChildCount(node);
				YGNodeInsertChild(node, subWidget->detachedNode, childCount);
			}
			else
			{
				YGNodeSetMeasureFunc(node, nullptr);

				auto childCount = YGNodeGetChildCount(node);
				YGNodeInsertChild(node, subWidget->node, childCount);
			}
		}
	}

	void CWidget::OnSubobjectAttached(Object* object)
	{
		Super::OnSubobjectAttached(object);

		if (!object)
			return;

		// DockSpace has its own SubWidget logic
		if (object->IsOfType<CWidget>() && !IsOfType<CDockSpace>() && IsSubWidgetAllowed(object->GetClass()))
		{
			CWidget* widget = (CWidget*)object;
			attachedWidgets.Add(widget);
			widget->parent = this;
			widget->styleSheet->parent = styleSheet;
			widget->ownerWindow = ownerWindow;

			if (widget->IsLayoutCalculationRoot())
			{
				// SubWidget requires independent layout calculation for its children.
				// Ex: Child window, Splitter view, etc. Insert a new root (detached) node for that.
				YGNodeSetMeasureFunc(node, nullptr);

				auto childCount = YGNodeGetChildCount(node);
				YGNodeInsertChild(node, widget->detachedNode, childCount);
			}
			else
			{
				YGNodeSetMeasureFunc(node, nullptr);

				auto childCount = YGNodeGetChildCount(node);
				YGNodeInsertChild(node, widget->node, childCount);
			}
		}

		SetNeedsStyle();
		SetNeedsLayout();
		SetNeedsPaint();
	}

	void CWidget::OnSubobjectDetached(Object* object)
	{
		Super::OnSubobjectDetached(object);

		if (!object)
			return;

		if (object->IsOfType<CWidget>() && !IsOfType<CDockSpace>() && IsSubWidgetAllowed(object->GetClass()))
		{
			CWidget* widget = (CWidget*)object;
			widget->parent = nullptr;
			widget->styleSheet->parent = nullptr;
			attachedWidgets.Remove(widget);
			widget->ownerWindow = nullptr;

			auto childCount = YGNodeGetChildCount(node);
			for (int i = 0; i < childCount; i++)
			{
				auto childNodeRef = YGNodeGetChild(node, i);
				CWidget* ctx = (CWidget*)YGNodeGetContext(childNodeRef);
				if (ctx != nullptr && ctx == widget)
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

		SetNeedsStyle();
		SetNeedsLayout();
		SetNeedsPaint();
	}

	void CWidget::SetVisible(bool visible)
	{
		if (this->visible == visible)
			return;

		this->visible = visible;
		SetNeedsPaint();
	}

	void CWidget::SetEnabled(bool enabled)
	{
		if (this->enabled == enabled)
			return;

		this->enabled = enabled;
		if (!enabled)
		{
			stateFlags = CStateFlag::Default;
		}

		SetNeedsLayout();
		SetNeedsStyle();
		SetNeedsPaint();

		if (parent)
		{
			parent->ClearChildNodes();
			parent->ReAddChildNodes();
		}
	}

	bool CWidget::IsEnabled() const
	{
		if (parent == nullptr)
			return enabled;

		return enabled && parent->IsEnabled();
	}

	bool CWidget::IsVisible() const
	{
		if (parent == nullptr)
			return enabled && visible;

		return enabled && visible && parent->IsVisible() && parent->IsEnabled();
	}

	bool CWidget::IsInteractable() const
	{
		if (!interactable)
			return false;

		if (parent != nullptr && !parent->IsInteractable())
			return false;

		return interactable;
	}

	void CWidget::SetInteractable(bool interactable)
	{
		this->interactable = interactable;
	}

	bool CWidget::IsSubWidgetAllowed(Class* subWidgetClass)
	{
		return subWidgetClass != nullptr && subWidgetClass->IsSubclassOf<CWidget>();
	}

	bool CWidget::IsWindow()
	{
		return IsOfType<CWindow>();
	}

	void CWidget::SetNeedsPaint()
	{
		needsPaint = true;

		for (int i = 0; i < attachedWidgets.GetSize(); ++i)
		{
			attachedWidgets[i]->SetNeedsPaint();
		}
	}

	void CWidget::SetNeedsLayout()
	{
		needsLayout = true;
		if (YGNodeGetChildCount(node) == 0)
		{
			if (!YGNodeHasMeasureFunc(node))
			{
				YGNodeSetMeasureFunc(node, MeasureFunctionCallback);
			}

			YGNodeMarkDirty(node);
		}

		YGNodeSetHasNewLayout(node, true);
		YGNodeSetHasNewLayout(detachedNode, true);

		for (int i = 0; i < attachedWidgets.GetSize(); ++i)
		{
			attachedWidgets[i]->SetNeedsLayout();
		}
	}

	void CWidget::ClearNeedsLayout()
	{
		needsLayout = false;

		for (int i = 0; i < attachedWidgets.GetSize(); ++i)
		{
			attachedWidgets[i]->ClearNeedsLayout();
		}
	}

	void CWidget::SetNeedsStyle()
	{
		if (needsStyle)
			return;

		needsStyle = true;
		needsPaint = true;

		for (int i = 0; i < attachedWidgets.GetSize(); ++i)
		{
			attachedWidgets[i]->SetNeedsStyle();
		}
	}

	bool CWidget::NeedsPaint()
	{
		if (needsPaint)
			return true;

		for (auto widget : attachedWidgets)
		{
			if (widget != this && widget != nullptr && widget->NeedsPaint())
				return true;
		}

		return false;
	}

	bool CWidget::NeedsStyle()
	{
		return needsStyle;
	}

	bool CWidget::NeedsLayout()
	{
		if (needsLayout)
			return true;

		for (CWidget* widget : attachedWidgets)
		{
			if (widget->NeedsLayout())
				return true;
		}

		return false;
	}

	void CWidget::UpdateLayoutIfNeeded()
	{
		if (NeedsLayout())
		{
			UpdateStyleIfNeeded();

			Vec2 availableSize = Vec2(YGUndefined, YGUndefined);

			if (IsOfType<CWindow>())
			{
				CWindow* window = (CWindow*)this;
				CWindow* parentWindow = nullptr;
				if (window->parent && window->parent->IsOfType<CWindow>())
					parentWindow = (CWindow*)window->parent;

				PlatformWindow* nativeWindow = window->nativeWindow;

				if (nativeWindow)
				{
					u32 w, h;
					nativeWindow->GetWindowSize(&w, &h);
					window->windowSize = Vec2(w, h);
					availableSize = window->windowSize;
					rootOrigin = Vec2(0, 0);
				}
				else if (parentWindow)
				{
					auto parentSize = parentWindow->windowSize;//parentWindow->GetComputedLayoutSize();
					if (parentSize == Vec2(0, 0))
						parentSize = parentWindow->GetComputedLayoutSize();
					window->windowSize = parentSize -
						Vec2(parentWindow->rootPadding.left + parentWindow->rootPadding.right,
							parentWindow->rootPadding.top + parentWindow->rootPadding.bottom);
					availableSize = window->windowSize;
				}
			}

			if (parent)
			{
				Vec2 parentSize = parent->GetComputedLayoutSize();
				availableSize = parentSize -
					Vec2(parent->rootPadding.left + parent->rootPadding.right,
						parent->rootPadding.top + parent->rootPadding.bottom);
			}

			if (IsLayoutCalculationRoot()) // Found a widget with independent layout calculation
			{
				YGNodeCalculateLayout(node, availableSize.width, availableSize.height, YGDirectionLTR);
			}

			if (IsOfType<CWindow>())
			{
				CWindow* window = (CWindow*)this;

				auto size = GetComputedLayoutSize();
				if (size != Vec2(0, 0) && !isnan(size.x) && !isnan(size.y))
				{
					window->windowSize = size;
				}
			}
			
			if (parent)
			{
				rootOrigin = parent->rootOrigin + parent->GetComputedLayoutTopLeft() + parent->rootPadding.min;
			}

			for (CWidget* widget : attachedWidgets)
			{
				widget->UpdateLayoutIfNeeded();
			}

			OnAfterUpdateLayout();

			needsLayout = false;
		}
	}

	bool CWidget::SubWidgetExistsRecursive(CWidget* subWidget)
	{
		if (!subWidget)
			return false;
		if (subWidget == this)
			return true;

		for (CWidget* widget : attachedWidgets)
		{
			if (widget->SubWidgetExistsRecursive(subWidget))
				return true;
		}

		return false;
	}

	bool CWidget::ParentWidgetExistsRecursive(CWidget* parentWidget)
	{
		if (!parentWidget)
			return false;
		if (parentWidget == this)
			return true;
		if (parent == nullptr)
			return false;
		return parent->ParentWidgetExistsRecursive(parentWidget);
	}

	void CWidget::UpdateStyleIfNeeded()
	{
		if (NeedsStyle())
		{
			CStyle prevComputedStyle = computedStyle;
			computedStyle = {};

			if (parent != nullptr)
			{
				const auto& parentComputedStyle = parent->computedStyle;
				const auto& inheritedProperties = CStyle::GetInheritedProperties();
				for (auto property : inheritedProperties)
				{
					if (parentComputedStyle.properties.KeyExists(property))
					{
						computedStyle.properties[property] = parentComputedStyle.properties.Get(property);
					}
				}
			}

			if (!IsInteractable())
			{
				stateFlags &= ~CStateFlag::Enabled;
				stateFlags |= CStateFlag::Disabled;
			}
			else
			{
				stateFlags |= CStateFlag::Enabled;
				stateFlags &= ~CStateFlag::Disabled;
			}
			
			auto selectStyle = styleSheet->SelectStyle(this, stateFlags, subControl);
			computedStyle.ApplyProperties(selectStyle);

			bool layoutChanged = computedStyle.CompareLayoutProperties(prevComputedStyle);

			OnBeforeComputeStyle();

			for (const auto& [property, value] : selectStyle.properties)
			{
				if (!value.IsValid())
					continue;

				if (property == CStylePropertyType::Cursor)
				{
					hoverCursor = static_cast<CCursor>(value.enumValue.x);
				}

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
				else if (property == CStylePropertyType::BorderWidth) // Do not apply border width for layout purposes
				{
					if (value.IsSingle())
					{
						//YGNodeStyleSetBorder(node, YGEdgeAll, value.single);
					}
					else if (value.IsVector())
					{
						// YGNodeStyleSetBorder(node, YGEdgeLeft, value.vector.left);
						// YGNodeStyleSetBorder(node, YGEdgeTop, value.vector.top);
						// YGNodeStyleSetBorder(node, YGEdgeRight, value.vector.right);
						// YGNodeStyleSetBorder(node, YGEdgeBottom, value.vector.bottom);
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

			if (layoutChanged)
			{
				SetNeedsLayout();
			}
		}

		for (CWidget* child : attachedWidgets)
		{
			child->UpdateStyleIfNeeded();
		}
	}

	void CWidget::AddSubWidget(CWidget* widget)
	{
		AttachSubobject(widget);
	}

	void CWidget::RemoveSubWidget(CWidget* widget)
	{
		DetachSubobject(widget);
	}

	Rect CWidget::GetScreenSpaceRect()
	{
		if (ownerWindow == nullptr)
		{
			if (IsWindow())
			{
				CWindow* window = (CWindow*)this;
				if (window->nativeWindow != nullptr)
				{
					Vec2i posInt = window->nativeWindow->GetWindowPosition();
					Vec2 pos = Vec2(posInt.x, posInt.y);
					u32 w, h;
					window->nativeWindow->GetWindowSize(&w, &h);

					return Rect::FromSize(pos + rootOrigin, Vec2(w, h));
				}
			}

			return {};
		}

		PlatformWindow* nativeWindow = ownerWindow->GetRootNativeWindow();
		if (nativeWindow == nullptr)
			return {};

		Vec2 scrollOffset = Vec2();
		if (parent != nullptr)
			scrollOffset = parent->normalizedScroll * (parent->contentSize - parent->GetComputedLayoutSize());

		{
			Vec2i posInt = nativeWindow->GetWindowPosition();
			Vec2 pos = Vec2(posInt.x, posInt.y);

			return Rect::FromSize(pos + rootOrigin + GetComputedLayoutTopLeft() - scrollOffset, GetComputedLayoutSize());
		}
	}

	Rect CWidget::LocalToScreenSpaceRect(const Rect& rect)
	{
		auto size = rect.GetSize();

		if (ownerWindow == nullptr)
		{
			if (IsWindow())
			{
				CWindow* window = (CWindow*)this;
				if (window->nativeWindow != nullptr)
				{
					Vec2i posInt = window->nativeWindow->GetWindowPosition();
					Vec2 pos = Vec2(posInt.x, posInt.y);
					u32 w, h;
					window->nativeWindow->GetWindowSize(&w, &h);

					return Rect::FromSize(pos + rootOrigin + rect.min, size);
				}
			}

			return rect;
		}

		PlatformWindow* nativeWindow = ownerWindow->GetRootNativeWindow();
		if (nativeWindow == nullptr)
			return rect;

		Vec2 scrollOffset = Vec2();
		if (parent != nullptr)
			scrollOffset = parent->normalizedScroll * (parent->contentSize - parent->GetComputedLayoutSize());

		{
			Vec2i posInt = nativeWindow->GetWindowPosition();
			Vec2 pos = posInt.ToVec2();

			return Rect::FromSize(pos + rootOrigin + GetComputedLayoutTopLeft() - scrollOffset + rect.min, size);
		}
	}

	Vec2 CWidget::ScreenSpaceToLocalPoint(const Vec2& point)
	{
		if (ownerWindow == nullptr)
		{
			if (IsWindow())
			{
				CWindow* window = (CWindow*)this;
				if (window->nativeWindow != nullptr)
				{
					Vec2 pos = window->nativeWindow->GetWindowPosition().ToVec2();

					return point - (pos + rootOrigin);
				}
			}

			return point;
		}

		PlatformWindow* nativeWindow = ownerWindow->GetRootNativeWindow();
		if (nativeWindow == nullptr)
			return point; // Should never happen

		Vec2 scrollOffset = Vec2();
		if (parent != nullptr)
			scrollOffset = parent->normalizedScroll * (parent->contentSize - parent->GetComputedLayoutSize());

		{
			Vec2 pos = nativeWindow->GetWindowPosition().ToVec2();

			return point - (pos + rootOrigin + GetComputedLayoutTopLeft() - scrollOffset);
		}
	}

	Rect CWidget::LocalToWindowSpaceRect(const Rect& rect)
	{
		auto size = rect.GetSize();

		if (ownerWindow == nullptr)
		{
			if (IsWindow())
			{
				CWindow* window = (CWindow*)this;
				if (window->nativeWindow != nullptr)
				{
					u32 w, h;
					window->nativeWindow->GetWindowSize(&w, &h);

					return Rect::FromSize(rootOrigin + rect.min, size);
				}
			}

			return rect;
		}

		Vec2 scrollOffset = Vec2();
		if (parent != nullptr)
			scrollOffset = parent->normalizedScroll * (parent->contentSize - parent->GetComputedLayoutSize());

		return Rect::FromSize(rootOrigin + GetComputedLayoutTopLeft() - scrollOffset + rect.min, size);
	}

	Renderer2D* CWidget::GetRenderer()
	{
		Renderer2D* renderer = nullptr;
		CWindow* owner = ownerWindow;
		while (owner != nullptr)
		{
			if (owner->ownerWindow == nullptr)
			{
				renderer = owner->renderer;
				break;
			}
			owner = owner->ownerWindow;
		}

		return renderer;
	}

	PlatformWindow* CWidget::GetNativeWindow()
	{
		if (!ownerWindow)
			return nullptr;
		return ownerWindow->GetRootNativeWindow();
	}

	void CWidget::QueueDestroy()
	{
		if (isQueuedForDestruction)
			return;
		isQueuedForDestruction = true;

		SetEnabled(false);

		if (parent)
		{
			parent->RemoveSubWidget(this);
		}
		else if (IsOfType<CWindow>())
		{
			CApplication::Get()->windows.Remove((CWindow*)this);
		}
		
		CApplication::Get()->destructionQueue.Add(this);
	}

	Vec2 CWidget::CalculateTextSize(const String& text, f32 fontSize, Name fontName, f32 width)
	{
		Renderer2D* renderer = nullptr;
		CWindow* owner = ownerWindow;
		while (owner != nullptr)
		{
			if (owner->ownerWindow == nullptr)
			{
				renderer = owner->renderer;
				break;
			}
			owner = owner->ownerWindow;
		}

		if (!renderer)
			return Vec2();

		return renderer->CalculateTextSize(text, fontSize, fontName, width);
	}

	void CWidget::Focus()
	{
		CApplication::Get()->SetFocus(this);
	}

	void CWidget::Unfocus()
	{
		CApplication::Get()->SetFocus(parent);
	}

	void CWidget::SetNeedsPaintRecursively(bool newValue)
	{
		needsPaint = newValue;

		for (auto widget : attachedWidgets)
		{
			if (widget != this && widget != nullptr)
				widget->SetNeedsPaintRecursively(newValue);
		}
	}

	void CWidget::Construct()
	{
		if (parent != nullptr)
		{
			styleSheet->parent = parent->styleSheet;
		}
	}

	void CWidget::OnPaint(CPaintEvent* paintEvent)
	{
		CPainter* painter = paintEvent->painter;

		Color bgColor = Color();
		Name bgImage = "";
		Color outlineColor = Color();
		f32 borderWidth = 0.0f;
		Vec4 borderRadius = Vec4();

		if (computedStyle.properties.KeyExists(CStylePropertyType::Background))
		{
			bgColor = computedStyle.properties[CStylePropertyType::Background].color;
		}

		if (computedStyle.properties.KeyExists(CStylePropertyType::BackgroundImage))
		{
			bgImage = computedStyle.properties[CStylePropertyType::BackgroundImage].string;
		}

		if (computedStyle.properties.KeyExists(CStylePropertyType::BorderColor))
		{
			outlineColor = computedStyle.properties[CStylePropertyType::BorderColor].color;
		}

		if (computedStyle.properties.KeyExists(CStylePropertyType::BorderWidth))
		{
			borderWidth = computedStyle.properties[CStylePropertyType::BorderWidth].single;
		}

		if (computedStyle.properties.KeyExists(CStylePropertyType::BorderRadius))
		{
			borderRadius = computedStyle.properties[CStylePropertyType::BorderRadius].vector;
		}

		CPen pen = CPen(); pen.SetColor(outlineColor); pen.SetWidth(borderWidth);
		CBrush brush = CBrush(); brush.SetColor(bgColor);
		painter->SetPen(pen);
		painter->SetBrush(brush);

		Rect rect = Rect::FromSize(GetComputedLayoutTopLeft(), GetComputedLayoutSize());

		if (borderRadius == Vec4(0, 0, 0, 0) && ((outlineColor.a > 0 && borderWidth > 0) || bgColor.a > 0))
		{
			painter->DrawRect(rect);
		}
		else if ((outlineColor.a > 0 && borderWidth > 0) || bgColor.a > 0)
		{
			painter->DrawRoundedRect(rect, borderRadius);
		}

		if (bgImage.IsValid())
		{
			RPI::Texture* texture = CApplication::Get()->LoadImage(bgImage);
			if (texture)
			{
				painter->DrawTexture(rect, texture);
			}
		}
	}

	void CWidget::OnPaintOverlay(CPaintEvent* paintEvent)
	{
		CPainter* painter = paintEvent->painter;


	}

	void CWidget::HandleEvent(CEvent* event)
	{
		if (event == nullptr)
			return;
		if (!IsEnabled())
			return;

		bool popPaintCoords = false;
		bool shouldPropagateDownwards = true;
		bool skipPaint = false;
		bool isButton = IsOfType<CButton>();

		if (event->type == CEventType::FocusChanged)
		{
			CFocusEvent* focusEvent = static_cast<CFocusEvent*>(event);
			focusEvent->Consume(this);

			if (focusEvent->GotFocus() && !IsFocussed())
			{
				OnFocusGained();
				OnFocused();
				//CE_LOG(Info, All, "Got Focus: {}", GetName());
			}
			else if (focusEvent->LostFocus() && IsFocussed())
			{
				OnFocusLost();
				OnUnfocused();
				//CE_LOG(Info, All, "Lost Focus: {}", GetName());
			}

			if (focusEvent->GotFocus() && !IsFocussed())
			{
				stateFlags |= CStateFlag::Focused;
				SetNeedsStyle();
				SetNeedsPaint();
			}
			else if (focusEvent->LostFocus() && IsFocussed())
			{
				stateFlags &= ~CStateFlag::Focused;
				SetNeedsStyle();
				SetNeedsPaint();
			}
		}

		// Paint event
		if (event->type == CEventType::PaintEvent)
		{
			CPaintEvent* paintEvent = (CPaintEvent*)event;
			paintEvent->direction = CEventDirection::TopToBottom;

			Vec2 scrollOffset = Vec2();

			if (parent != nullptr)
				scrollOffset = parent->normalizedScroll * (parent->contentSize - parent->GetComputedLayoutSize());

			if (paintEvent->painter != nullptr && CanPaint() && IsVisible() && IsEnabled())
			{
				paintEvent->painter->Reset();
				popPaintCoords = true;
				Vec2 origin = Vec2();
				if (parent != nullptr)
				{
					origin = parent->GetComputedLayoutTopLeft() + parent->rootPadding.min;
				}
				paintEvent->painter->PushChildCoordinateSpace(origin - scrollOffset);
				if (clipChildren)
				{
					auto contentRect = Rect::FromSize(GetComputedLayoutTopLeft(), GetComputedLayoutSize());
					if (paintEvent->painter->ClipRectExists())
					{
						Rect windowSpaceContentRect = LocalToWindowSpaceRect(contentRect);
						Rect prevClipRect = paintEvent->painter->GetLastClipRect();
					}
					paintEvent->painter->PushClipRect(contentRect);
				}

				if (!skipPaint)
				{
					OnPaint(paintEvent);
				}
			}
			else
			{
				shouldPropagateDownwards = false;
			}
		}

		// Mouse events
		if (receiveMouseEvents && event->IsMouseEvent() && !event->IsDragEvent() && (!event->isConsumed || event->firstConsumer == this))
		{
			CMouseEvent* mouseEvent = (CMouseEvent*)event;
			
			if (event->type == CEventType::MousePress && mouseEvent->button == MouseButton::Left)
			{
				mouseEvent->Consume(this);
				stateFlags |= CStateFlag::Pressed;
				isPressed = true;
				SetNeedsStyle();
			}
			else if (event->type == CEventType::MouseRelease && mouseEvent->button == MouseButton::Left)
			{
				mouseEvent->Consume(this);
				stateFlags &= ~CStateFlag::Pressed;
				isPressed = false;
				SetNeedsStyle();
			}

			if (event->type == CEventType::MouseEnter && (mouseEvent->button == MouseButton::None || isPressed))
			{
				mouseEvent->Consume(this);
				stateFlags |= CStateFlag::Hovered;
				if (isPressed)
				{
					stateFlags |= CStateFlag::Pressed;
				}
				if (hoverCursor != CCursor::Inherited)
				{
					CApplication::Get()->PushCursor(hoverCursor);
				}
				SetNeedsStyle();
			}
			else if (event->type == CEventType::MouseMove && mouseEvent->button == MouseButton::None && !EnumHasFlag(stateFlags, CStateFlag::Hovered))
			{
				mouseEvent->Consume(this);
				stateFlags |= CStateFlag::Hovered;
				if (isPressed)
				{
					stateFlags |= CStateFlag::Pressed;
				}
				SetNeedsStyle();
			}
			else if (event->type == CEventType::MouseLeave)
			{
				mouseEvent->Consume(this);
				stateFlags &= ~CStateFlag::Hovered;
				if (isPressed)
				{
					stateFlags &= ~CStateFlag::Pressed;
				}
				if (hoverCursor != CCursor::Inherited)
				{
					CApplication::Get()->PopCursor();
				}
				SetNeedsStyle();
			}
		}
		
		if (event->direction == CEventDirection::TopToBottom && IsEnabled()) // Pass event down the chain
		{
			if (!shouldPropagateDownwards)
			{
				return;
			}

			for (CWidget* widget : attachedWidgets)
			{
				if (event->stopPropagation)
				{
					return;
				}
				widget->HandleEvent(event);
			}
		}
		else if (event->direction == CEventDirection::BottomToTop) // Pass event up the chain
		{
			if (event->stopPropagation)
			{
				return;
			}
			if (parent != nullptr)
			{
				parent->HandleEvent(event);
			}
		}

		if (event->type == CEventType::PaintEvent)
		{
			CPaintEvent* paintEvent = (CPaintEvent*)event;
			if (paintEvent->painter != nullptr)
			{
				OnPaintOverlay(paintEvent);
				if (clipChildren)
				{
					paintEvent->painter->PopClipRect();
				}
				if (popPaintCoords)
				{
					paintEvent->painter->PopChildCoordinateSpace();
				}
			}
		}
	}

	void CWidget::OnBeforeDestroy()
	{
		Super::OnBeforeDestroy();

		CApplication* app = CApplication::TryGet();
		if (app)
		{
			app->OnWidgetDestroyed(this);
		}

		if (IsWindow())
		{
			CWindow* window = (CWindow*)this;
			if (window->nativeWindow != nullptr)
			{
				PlatformApplication::Get()->DestroyWindow(window->nativeWindow);
				window->nativeWindow = nullptr;
			}
		}
	}
    
} // namespace CE::Widgets

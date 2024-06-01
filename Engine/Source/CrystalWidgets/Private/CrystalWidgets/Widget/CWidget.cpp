#include "CrystalWidgets.h"

#define TRACY_ZONE\
	ZoneScoped;\
	ZoneTextF(GetName().GetCString());
	

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
			if (!subWidget->IsSelfEnabled())
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

		if (object->IsOfType<CBehavior>())
		{
			CBehavior* behavior = static_cast<CBehavior*>(object);
			behavior->self = this;
			behaviors.Add(behavior);
		}

		// DockSpace has its own SubWidget logic
		if (object->IsOfType<CWidget>() && !IsOfType<CDockSpace>() && IsSubWidgetAllowed(object->GetClass()) && IsContainer())
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
		else if (object->IsOfType<CWidget>())
		{
			CWidget* widget = (CWidget*)object;
			widget->styleSheet->parent = styleSheet;
			widget->ownerWindow = ownerWindow;
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

		if (object->IsOfType<CBehavior>())
		{
			CBehavior* behavior = static_cast<CBehavior*>(object);
			behavior->self = nullptr;
			behaviors.Remove(behavior);
		}

		if (object->IsOfType<CWidget>() && !IsOfType<CDockSpace>() && IsSubWidgetAllowed(object->GetClass()) && IsContainer())
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
		else if (object->IsOfType<CWidget>())
		{
			CWidget* widget = (CWidget*)object;
			widget->styleSheet->parent = nullptr;
			widget->ownerWindow = nullptr;
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
		TRACY_ZONE;

		if (this->enabled == enabled)
			return;

		this->enabled = enabled;
		if (!enabled)
		{
			stateFlags = CStateFlag::Default;
		}

		if (parent)
		{
			parent->ReAddChildNodes();
		}

		if (enabled)
			OnEnabled();
		else
			OnDisabled();

		SetNeedsLayout();
		SetNeedsStyle();
		SetNeedsPaint();
	}

	bool CWidget::IsEnabled() const
	{
		TRACY_ZONE;

		if (parent == nullptr)
			return enabled;

		return enabled && parent->IsEnabled();
	}

	bool CWidget::IsVisible() const
	{
		TRACY_ZONE;

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

		return interactable && IsVisible();
	}

	void CWidget::SetInteractable(bool interactable)
	{
		if (this->interactable == interactable)
			return;

		this->interactable = interactable;
		SetNeedsStyle();
		SetNeedsPaint();
	}

	void CWidget::SetRotation(f32 rotation)
	{
		if (this->rotation == rotation)
			return;

		this->rotation = rotation;

		SetNeedsPaint();
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
	}

	void CWidget::SetNeedsLayout()
	{
		TRACY_ZONE;

		needsLayout = true;

		if (YGNodeGetChildCount(node) == 0)
		{
			if (!YGNodeHasMeasureFunc(node))
			{
				YGNodeSetMeasureFunc(node, MeasureFunctionCallback);
			}

			YGNodeMarkDirty(node);
		}

		for (int i = 0; i < attachedWidgets.GetSize(); ++i)
		{
			attachedWidgets[i]->SetNeedsLayout();
		}
	}

	void CWidget::SetNeedsStyle()
	{
		TRACY_ZONE;

		if (needsStyle)
			return;

		needsStyle = true;
		needsPaint = true;

		for (int i = 0; i < attachedWidgets.GetSize(); ++i)
		{
			attachedWidgets[i]->SetNeedsStyle();
		}

		if (IsWindow())
		{
			CWindow* window = static_cast<CWindow*>(this);

			for (CWindow* attachedWindow : window->attachedWindows)
			{
				attachedWindow->SetNeedsStyle();
			}
		}
	}

	bool CWidget::NeedsPaint()
	{
		TRACY_ZONE;

		if (needsPaint)
			return true;

		for (auto widget : attachedWidgets)
		{
			if (widget != this && widget != nullptr && widget->NeedsPaint())
				return true;
		}

		if (IsWindow())
		{
			CWindow* window = static_cast<CWindow*>(this);

			for (CWindow* attachedWindow : window->attachedWindows)
			{
				if (attachedWindow != this && attachedWindow->NeedsPaint())
					return true;
			}
		}

		return false;
	}

	bool CWidget::NeedsStyle()
	{
		return needsStyle;
	}

	void CWidget::LoadStyleSheet(const IO::Path& fullPath)
	{
		TRACY_ZONE;

		String css = "";
		FileStream reader = FileStream(fullPath, Stream::Permissions::ReadOnly);
		if (!reader.IsOpen())
			return;

		u64 length = reader.GetLength();
		css.Reserve(length + 1);
		reader.Read(css.GetData(), length);
		css.UpdateLength();

		CSSParser::ParseStyleSheet((CSSStyleSheet*)styleSheet, css);
	}

	bool CWidget::NeedsLayout()
	{
		TRACY_ZONE;

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
		TRACY_ZONE;

		if (NeedsLayout())
		{
			Vec4 rootPadding = GetFinalRootPadding();

			Vec2 availableSize = Vec2(YGUndefined, YGUndefined);

			bool isMenu = IsOfType<CMenu>();

			if (IsOfType<CWindow>())
			{
				CWindow* window = (CWindow*)this;
				CWindow* parentWindow = nullptr;
				if (window->parent && window->parent->IsOfType<CWindow>())
					parentWindow = (CWindow*)window->parent;

				CPlatformWindow* nativeWindow = window->nativeWindow;

				if (nativeWindow)
				{
					u32 w, h;
					nativeWindow->GetWindowSize(&w, &h);
					window->windowSize = Vec2(w, h);
					availableSize = window->windowSize - Vec2(rootPadding.left + rootPadding.right, rootPadding.top + rootPadding.bottom);
					rootOrigin = Vec2(0, 0);
				}
				else if (parentWindow)
				{
					auto parentSize = parentWindow->windowSize;//parentWindow->GetComputedLayoutSize();
					if (parentSize == Vec2(0, 0))
						parentSize = parentWindow->GetComputedLayoutSize();
					auto parentWindowRootPadding = parentWindow->GetFinalRootPadding();

					window->windowSize = parentSize -
						Vec2(parentWindowRootPadding.left + parentWindowRootPadding.right,
							parentWindowRootPadding.top + parentWindowRootPadding.bottom);
					availableSize = window->windowSize;
				}
			}

			if (parent)
			{
				auto parentRootPadding = parent->GetFinalRootPadding();

				Vec2 parentSize = parent->GetAvailableSizeForChild(this);
				availableSize = parentSize -
					Vec2(parentRootPadding.left + parentRootPadding.right,
						parentRootPadding.top + parentRootPadding.bottom);

				// TODO: Remove the below block if it causes layout bugs because it is a 'hack'
				if (!IsOfType<CDockSpace>() && !IsOfType<CDockSplitView>())
				{
					availableSize -= Vec2(rootPadding.left + rootPadding.right,
						rootPadding.top + rootPadding.bottom);
				}
			}

			if (IsLayoutCalculationRoot()) // Found a widget with independent layout calculation
			{
				YGNodeCalculateLayout(node, availableSize.width, availableSize.height, YGDirectionLTR);
			}

			if (IsOfType<CWindow>())
			{
				auto size = GetComputedLayoutSize();
				CWindow* window = (CWindow*)this;

				if (size != Vec2(0, 0) && !isnan(size.x) && !isnan(size.y))
				{
					window->windowSize = size;
				}
			}
			
			if (parent)
			{
				rootOrigin = parent->rootOrigin + parent->GetComputedLayoutTopLeft() + parent->GetFinalRootPadding().min;
			}

			for (CWidget* widget : attachedWidgets)
			{
				widget->UpdateLayoutIfNeeded();
			}

			if (IsWindow())
			{
				CWindow* window = static_cast<CWindow*>(this);

				for (CWindow* attachedWindow : window->attachedWindows)
				{
					attachedWindow->UpdateLayoutIfNeeded();
				}
			}

			OnAfterUpdateLayout();

			needsLayout = false;
		}
	}

	void CWidget::OnAfterUpdateLayout()
	{
		TRACY_ZONE;

		Vec2 originalSize = GetComputedLayoutSize();
		f32 contentMaxY = originalSize.height;
		f32 contentMaxX = originalSize.width;

		for (CWidget* widget : attachedWidgets)
		{
			if (!widget->IsEnabled())
				continue;

			Vec2 pos = widget->GetComputedLayoutTopLeft();
			Vec2 size = widget->GetComputedLayoutSize();
			f32 maxY = pos.y + size.height;
			f32 maxX = pos.x + size.width;
			if (isnan(maxX))
				maxX = 0;
			if (isnan(maxY))
				maxY = 0;

			contentMaxY = Math::Max(contentMaxY, maxY);
			contentMaxX = Math::Max(contentMaxX, maxX);
		}

		contentSize = Vec2(contentMaxX, contentMaxY);
	}

	Vec2 CWidget::CalculateIntrinsicSize(f32 width, f32 height)
	{
		return Vec2();
	}

	void CWidget::OnPaintEarly(CPaintEvent* paintEvent)
	{

	}

	CBehavior* CWidget::AddBehavior(SubClass<CBehavior> behaviorClass)
	{
		if (behaviorClass == nullptr)
			return nullptr;

		for (CBehavior* behavior : behaviors)
		{
			if (behavior->GetClass() == behaviorClass) // Same behavior class already exists!
				return nullptr;
		}

		return CreateObject<CBehavior>(this, behaviorClass->GetName().GetLastComponent(), OF_Transient, behaviorClass);
	}

	CBehavior* CWidget::GetBehavior(SubClass<CBehavior> behaviorClass)
	{
		if (behaviorClass == nullptr)
			return nullptr;

		for (CBehavior* behavior : behaviors)
		{
			if (behavior->GetClass() == behaviorClass) // Same behavior class already exists!
				return behavior;
		}

		return nullptr;
	}

	Vec4 CWidget::GetFinalRootPadding()
	{
		CScrollBehavior* scrollBehavior = GetBehavior<CScrollBehavior>();
		if (scrollBehavior != nullptr && allowVerticalScroll && scrollBehavior->IsVerticalScrollVisible())
		{
			auto app = CApplication::Get();
			return rootPadding + Vec4(0, 0, app->styleConstants.scrollRectWidth, 0);
		}

		return rootPadding;
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

	static f32 maxStyleTime = 0.0f;

	void CWidget::UpdateStyleIfNeeded()
	{
		TRACY_ZONE;

		bool neededStyle = NeedsStyle();

		if (neededStyle)
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

			bool isRow = IsOfType<CTreeWidgetRow>();

			CStyleSheet* globalStyleSheet = CApplication::Get()->globalStyleSheet;

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
					constexpr f32 multiplier = 0.0f;
					if (value.IsSingle())
					{
						YGNodeStyleSetBorder(node, YGEdgeAll, value.single * multiplier);
					}
					else if (value.IsVector())
					{
						YGNodeStyleSetBorder(node, YGEdgeLeft, value.vector.left * multiplier);
						YGNodeStyleSetBorder(node, YGEdgeTop, value.vector.top * multiplier);
						YGNodeStyleSetBorder(node, YGEdgeRight, value.vector.right * multiplier);
						YGNodeStyleSetBorder(node, YGEdgeBottom, value.vector.bottom * multiplier);
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

			layoutChanged = PostComputeStyle() || layoutChanged;

			needsStyle = false;

			if (layoutChanged && !needsLayout)
			{
				SetNeedsLayout();
			}
		}

		for (CWidget* child : attachedWidgets)
		{
			child->UpdateStyleIfNeeded();
		}

		if (IsWindow())
		{
			CWindow* window = static_cast<CWindow*>(this);

			for (CWindow* attachedWindow : window->attachedWindows)
			{
				attachedWindow->UpdateStyleIfNeeded();
			}
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

	Vec2 CWidget::GetComputedLayoutTopLeft()
	{
		return Vec2(YGNodeLayoutGetLeft(node), YGNodeLayoutGetTop(node));
	}

	Vec2 CWidget::GetComputedLayoutSize()
	{
		return Vec2(YGNodeLayoutGetWidth(node), YGNodeLayoutGetHeight(node));
	}

	Vec2 CWidget::GetAvailableSizeForChild(CWidget* childWidget)
	{
		return GetComputedLayoutSize();
	}


	Rect CWidget::GetScreenSpaceRect()
	{
		if (IsWindow())
		{
			CWindow* window = (CWindow*)this;
			if (window->nativeWindow != nullptr)
			{
				Vec2i posInt = window->nativeWindow->GetPlatformWindow()->GetWindowPosition();
				Vec2 pos = Vec2(posInt.x, posInt.y);
				u32 w, h;
				window->nativeWindow->GetWindowSize(&w, &h);

				return Rect::FromSize(pos + rootOrigin, Vec2(w, h));
			}
		}

		PlatformWindow* nativeWindow = ownerWindow->GetRootNativeWindow()->GetPlatformWindow();
		if (nativeWindow == nullptr)
			return {};

		Vec2 scrollOffset = Vec2();
		CWidget* parentWidget = parent;

		while (parentWidget != nullptr)
		{
			scrollOffset += parentWidget->normalizedScroll * (parentWidget->contentSize - parentWidget->GetComputedLayoutSize());

			parentWidget = parentWidget->parent;
		}

		{
			Vec2i posInt = nativeWindow->GetWindowPosition();
			Vec2 pos = Vec2(posInt.x, posInt.y);

			return Rect::FromSize(pos + rootOrigin + GetComputedLayoutTopLeft() - scrollOffset, GetComputedLayoutSize());
		}
	}

	Rect CWidget::GetWindowSpaceRect()
	{
		if (IsWindow())
		{
			CWindow* window = (CWindow*)this;
			if (window->nativeWindow != nullptr)
			{
				u32 w, h;
				window->nativeWindow->GetWindowSize(&w, &h);

				return Rect::FromSize(rootOrigin, Vec2(w, h));
			}
		}

		PlatformWindow* nativeWindow = ownerWindow->GetRootNativeWindow()->GetPlatformWindow();
		if (nativeWindow == nullptr)
			return {};

		Vec2 scrollOffset = Vec2();
		CWidget* parentWidget = parent;

		while (parentWidget != nullptr)
		{
			scrollOffset += parentWidget->normalizedScroll * (parentWidget->contentSize - parentWidget->GetComputedLayoutSize());

			parentWidget = parentWidget->parent;
		}

		return Rect::FromSize(rootOrigin + GetComputedLayoutTopLeft() - scrollOffset, GetComputedLayoutSize());
	}

	Vec2 CWidget::LocalToScreenSpacePos(const Vec2& point)
	{
		if (ownerWindow == nullptr)
		{
			if (IsWindow())
			{
				CWindow* window = (CWindow*)this;
				if (window->nativeWindow != nullptr)
				{
					Vec2i posInt = window->nativeWindow->GetPlatformWindow()->GetWindowPosition();
					Vec2 pos = Vec2(posInt.x, posInt.y);
					u32 w, h;
					window->nativeWindow->GetWindowSize(&w, &h);

					return pos + rootOrigin + point;
				}
			}

			return point;
		}

		PlatformWindow* nativeWindow = ownerWindow->GetRootNativeWindow()->GetPlatformWindow();
		if (nativeWindow == nullptr)
			return point;

		Vec2 scrollOffset = Vec2();
		if (parent != nullptr)
			scrollOffset = parent->normalizedScroll * (parent->contentSize - parent->GetComputedLayoutSize());

		{
			Vec2i posInt = nativeWindow->GetWindowPosition();
			Vec2 pos = posInt.ToVec2();

			return pos + rootOrigin + GetComputedLayoutTopLeft() - scrollOffset + point;
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
					Vec2i posInt = window->nativeWindow->GetPlatformWindow()->GetWindowPosition();
					Vec2 pos = Vec2(posInt.x, posInt.y);
					u32 w, h;
					window->nativeWindow->GetWindowSize(&w, &h);

					return Rect::FromSize(pos + rootOrigin + rect.min, size);
				}
			}

			return rect;
		}

		PlatformWindow* nativeWindow = ownerWindow->GetRootNativeWindow()->GetPlatformWindow();
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

	Vec2 CWidget::LocalToWindowSpacePos(const Vec2& point)
	{
		if (ownerWindow == nullptr)
		{
			if (IsWindow())
			{
				CWindow* window = (CWindow*)this;
				if (window->nativeWindow != nullptr)
				{
					u32 w, h;
					window->nativeWindow->GetWindowSize(&w, &h);

					return rootOrigin + point;
				}
			}

			return point;
		}

		Vec2 scrollOffset = Vec2();

		if (parent != nullptr)
			scrollOffset = parent->normalizedScroll * (parent->contentSize - parent->GetComputedLayoutSize());

		return rootOrigin + GetComputedLayoutTopLeft() - scrollOffset + point;
	}

	Vec2 CWidget::ScreenToLocalSpacePoint(const Vec2& point)
	{
		if (ownerWindow == nullptr)
		{
			if (IsWindow())
			{
				CWindow* window = (CWindow*)this;
				if (window->nativeWindow != nullptr)
				{
					Vec2 pos = window->nativeWindow->GetPlatformWindow()->GetWindowPosition().ToVec2();

					return point - (pos + rootOrigin);
				}
			}

			return point;
		}

		PlatformWindow* nativeWindow = ownerWindow->GetRootNativeWindow()->GetPlatformWindow();
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

	Vec2 CWidget::ScreenToWindowSpacePoint(const Vec2& point)
	{
		if (ownerWindow == nullptr)
		{
			if (IsWindow())
			{
				CWindow* window = (CWindow*)this;
				if (window->nativeWindow != nullptr)
				{
					Vec2 pos = window->nativeWindow->GetPlatformWindow()->GetWindowPosition().ToVec2();

					return point - (pos);
				}
			}

			return point;
		}

		PlatformWindow* nativeWindow = ownerWindow->GetRootNativeWindow()->GetPlatformWindow();
		if (nativeWindow == nullptr)
			return point; // Should never happen

		Vec2 scrollOffset = Vec2();
		if (parent != nullptr)
			scrollOffset = parent->normalizedScroll * (parent->contentSize - parent->GetComputedLayoutSize());

		{
			Vec2 pos = nativeWindow->GetWindowPosition().ToVec2();

			return point - (pos - scrollOffset);
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
				if (owner->nativeWindow)
					renderer = owner->nativeWindow->renderer;
				break;
			}
			owner = owner->ownerWindow;
		}

		return renderer;
	}

	CPlatformWindow* CWidget::GetNativeWindow()
	{
		if (IsWindow())
		{
			CWindow* window = static_cast<CWindow*>(this);
			if (window->nativeWindow)
				return window->nativeWindow;
		}

		if (!parent)
			return nullptr;
		return parent->GetNativeWindow();
	}

	CWindow* CWidget::GetRootWindow()
	{
		if (IsWindow() && ownerWindow == nullptr)
		{
			return (CWindow*)this;
		}

		if (ownerWindow)
			return ownerWindow->GetRootWindow();

		if (parent)
			return parent->GetRootWindow();

		return nullptr;
	}

	void CWidget::QueueDestroy()
	{
		if (isQueuedForDestruction)
			return;
		isQueuedForDestruction = true;

		OnDestroyQueued();

		SetEnabled(false);

		if (IsWindow())
		{
			CWindow* window = static_cast<CWindow*>(this);
			window->Hide();
		}

		if (parent)
		{
			parent->RemoveSubWidget(this);
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
				renderer = owner->nativeWindow->renderer;
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

	static void RotatePoint(float x, float y, float centerX, float centerY, double angle, float& newX, float& newY) {
		float radians = angle * (M_PI / 180.0); // Convert degrees to radians
		float cosine = std::cos(radians);
		float sine = std::sin(radians);

		newX = centerX + (x - centerX) * cosine - (y - centerY) * sine;
		newY = centerY + (x - centerX) * sine + (y - centerY) * cosine;
	}

	// Function to find the bounding box of a rotated rectangle
	static Rect ComputeBoundingBox(const Rect& rectangle, float angle) {
		float x1 = rectangle.min.x;
		float y1 = rectangle.min.y;
		float x2 = rectangle.max.x;
		float y2 = rectangle.min.y;
		float x3 = rectangle.max.x;
		float y3 = rectangle.max.y;
		float x4 = rectangle.min.x;
		float y4 = rectangle.max.y;

		float centerX = (rectangle.min.x + rectangle.max.x) / 2.0f;
		float centerY = (rectangle.min.y + rectangle.max.y) / 2.0f;

		float newX1, newY1, newX2, newY2, newX3, newY3, newX4, newY4;

		RotatePoint(x1, y1, centerX, centerY, angle, newX1, newY1);
		RotatePoint(x2, y2, centerX, centerY, angle, newX2, newY2);
		RotatePoint(x3, y3, centerX, centerY, angle, newX3, newY3);
		RotatePoint(x4, y4, centerX, centerY, angle, newX4, newY4);

		float minX = std::min({ newX1, newX2, newX3, newX4 });
		float maxX = std::max({ newX1, newX2, newX3, newX4 });
		float minY = std::min({ newY1, newY2, newY3, newY4 });
		float maxY = std::max({ newY1, newY2, newY3, newY4 });

		Rect boundingBox = { minX, minY, maxX - minX, maxY - minY };
		return boundingBox;
	}

	void CWidget::OnPaint(CPaintEvent* paintEvent)
	{
		TRACY_ZONE;

		CPainter* painter = paintEvent->painter;

		Color bgColor = Color();
		Name bgImage = "";
		CGradient gradient{};
		Color outlineColor = Color();
		f32 borderWidth = 0.0f;
		Vec4 borderRadius = Vec4();

		if (computedStyle.properties.KeyExists(CStylePropertyType::Background))
		{
			bgColor = computedStyle.properties[CStylePropertyType::Background].color;
		}
		
		if (computedStyle.properties.KeyExists(CStylePropertyType::BackgroundImage) &&
			computedStyle.properties[CStylePropertyType::BackgroundImage].IsString())
		{
			bgImage = computedStyle.properties[CStylePropertyType::BackgroundImage].string;
		}

		if (backgroundImageOverride.IsValid())
		{
			bgImage = backgroundImageOverride;
		}

		if (computedStyle.properties.KeyExists(CStylePropertyType::BackgroundImage) &&
			computedStyle.properties[CStylePropertyType::BackgroundImage].IsGradient())
		{
			gradient = computedStyle.properties[CStylePropertyType::BackgroundImage].gradient;
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

		Color foreground = computedStyle.GetForegroundColor();

		CPen pen = CPen();// pen.SetColor(outlineColor); pen.SetWidth(borderWidth);
		CBrush brush = CBrush(); brush.SetColor(bgColor);
		if (gradient.keys.NonEmpty())
		{
			brush.SetGradient(gradient);
		}

		painter->SetPen(pen);
		painter->SetBrush(brush);

		Vec2 extraSize = Vec2();
		if (IsWindow() && parent == nullptr)
		{
			extraSize = Vec2(rootPadding.left + rootPadding.right, rootPadding.top + rootPadding.bottom);
		}

		const Vec2 totalSize = GetComputedLayoutSize() + extraSize;

		Rect rect = Rect::FromSize(GetComputedLayoutTopLeft(), totalSize);
		Rect bgDrawRect = rect;

		if (borderWidth > 0.1f)
		{
			bgDrawRect = Rect::FromSize(rect.min + Vec2(1, 1) * borderWidth * 0.25f,
				rect.GetSize() - Vec2(1, 1) * borderWidth * 0.5f);
		}

		painter->SetRotation(rotation);

		if (borderRadius == Vec4(0, 0, 0, 0) && (bgColor.a > 0 || gradient.keys.NonEmpty()))
		{
			painter->DrawRect(bgDrawRect);
		}
		else if (bgColor.a > 0 || gradient.keys.NonEmpty())
		{
			painter->DrawRoundedRect(bgDrawRect, borderRadius);
		}

		// Draw Background Image
		if (bgImage.IsValid() && canDrawBgImage)
		{
			RPI::Texture* texture = CApplication::Get()->LoadImage(bgImage);
			if (texture)
			{
				CBackgroundSize backgroundSize = computedStyle.GetBackgroundSize();
				CTextAlign backgroundPosition = computedStyle.GetBackgroundPosition();
				CBackgroundRepeat backgroundRepeat = computedStyle.GetBackgroundRepeat();

				Vec2 imageSize = Vec2(texture->GetWidth(), texture->GetHeight());
				f32 imageAspectRatio = imageSize.width / imageSize.height;
				f32 rectAspectRatio = totalSize.width / totalSize.height;

				Rect fullRect = Rect::FromSize(Vec2(), totalSize);
				if (borderWidth > 0.1f)
				{
					fullRect = Rect::FromSize(fullRect.min + Vec2(1, 1) * borderWidth * 0.25f,
						fullRect.GetSize() - Vec2(1, 1) * borderWidth * 0.5f);
				}

				painter->PushChildCoordinateSpace(GetComputedLayoutTopLeft());
				Rect clipRect = Rect::FromSize(Vec2(), totalSize);
				if (abs(rotation) > 0.01f)
				{
					clipRect = ComputeBoundingBox(clipRect, rotation);
				}
				painter->PushClipRect(clipRect, borderRadius); // Texture clip rect

				brush.SetColor(foreground);
				painter->SetBrush(brush);

				Rect textureRect;

				if (backgroundSize == CBackgroundSize::Cover)
				{
					// If the rectangle aspect ratio is greater than the image aspect ratio
					if (rectAspectRatio > imageAspectRatio)
					{
						Vec2 drawSize = Vec2(totalSize.width, totalSize.width / imageAspectRatio);
						Vec2 drawPos = Vec2(0, (totalSize.height - drawSize.height) / 2);
						textureRect = Rect::FromSize(drawPos, drawSize);
					}
					else
					{
						Vec2 drawSize = Vec2(totalSize.height * imageAspectRatio, totalSize.height);
						Vec2 drawPos = Vec2((totalSize.width - drawSize.width) / 2, 0);
						textureRect = Rect::FromSize(drawPos, drawSize);
					}
				}
				else if (backgroundSize == CBackgroundSize::Contain)
				{
					// If the rectangle is wider than the image
					if (rectAspectRatio > imageAspectRatio)
					{
						Vec2 drawSize = Vec2(totalSize.height * imageAspectRatio, totalSize.height);
						Vec2 drawPos = Vec2((totalSize.width - drawSize.width) / 2, 0);

						switch (backgroundPosition)
						{
						case CTextAlign::TopLeft:
						case CTextAlign::MiddleLeft:
						case CTextAlign::BottomLeft:
							drawPos.x = 0;
							break;
						case CTextAlign::TopRight:
						case CTextAlign::MiddleRight:
						case CTextAlign::BottomRight:
							drawPos.x *= 2;
							break;
						default:
							break; 
						}

						textureRect = Rect::FromSize(drawPos + Vec2(1, 1) * borderWidth * 0.5f, 
							drawSize - Vec2(1, 1) * borderWidth);
					}
					else
					{
						Vec2 drawSize = Vec2(totalSize.width, totalSize.width / imageAspectRatio);
						Vec2 drawPos = Vec2(0, (totalSize.height - drawSize.height) / 2);

						switch (backgroundPosition)
						{
						case CTextAlign::TopLeft:
						case CTextAlign::TopCenter:
						case CTextAlign::TopRight:
							drawPos.y = 0;
							break;
						case CTextAlign::BottomLeft:
						case CTextAlign::BottomCenter:
						case CTextAlign::BottomRight:
							drawPos.y *= 2;
							break;
						default:
							break;
						}

						textureRect = Rect::FromSize(drawPos + Vec2(1, 1) * borderWidth * 0.25f, 
							drawSize - Vec2(1, 1) * borderWidth * 0.5f);
					}
				}
				else // Fill
				{
					textureRect = Rect::FromSize(Vec2() + Vec2(1, 1) * borderWidth * 0.25f, 
						totalSize - Vec2(1, 1) * borderWidth * 0.5f);
				}

				if (backgroundRepeat == CBackgroundRepeat::NoRepeat)
				{
					painter->DrawTexture(textureRect, texture);
				}
				else
				{
					Rect drawRect = textureRect;
					Vec2 scale = Vec2(1, 1);
					Vec2 offset = Vec2();

					if (EnumHasFlag(backgroundRepeat, CBackgroundRepeat::RepeatX))
					{
						drawRect.min.x = fullRect.min.x;
						drawRect.max.x = fullRect.max.x;

						scale.x = 1 + drawRect.GetSize().width / textureRect.GetSize().width;
					}
					if (EnumHasFlag(backgroundRepeat, CBackgroundRepeat::RepeatY))
					{
						drawRect.min.y = fullRect.min.y;
						drawRect.max.y = fullRect.max.y;

						scale.y = Math::Max(1.0f, drawRect.GetSize().height / textureRect.GetSize().height);
					}

					offset.x = (textureRect.min.x - fullRect.min.x) / fullRect.GetSize().width;
					offset.y = (textureRect.min.y - fullRect.min.y) / fullRect.GetSize().height;

					painter->DrawTexture(drawRect, texture, backgroundRepeat, scale, offset);
				}

				painter->PopClipRect();
				painter->PopChildCoordinateSpace();
			}
		}

		OnPaintEarly(paintEvent);

		for (CBehavior* behavior : behaviors)
		{
			behavior->OnPaintEarly(painter);
		}

		// - Draw Border -

		pen = CPen(); pen.SetColor(outlineColor); pen.SetWidth(borderWidth);
		brush = CBrush();
		painter->SetPen(pen);
		painter->SetBrush(brush);

		Rect borderRect = rect;

		if (borderWidth > 0.01f && outlineColor.a > 0.001f)
		{
			if (borderRadius == Vec4(0, 0, 0, 0))
			{
				painter->DrawRect(borderRect);
			}
			else if ((outlineColor.a > 0 && borderWidth > 0) || bgColor.a > 0)
			{
				painter->DrawRoundedRect(borderRect, borderRadius);
			}
		}

		for (CBehavior* behavior : behaviors)
		{
			behavior->OnPaint(painter);
		}

		painter->SetRotation(0);
	}

	void CWidget::OnPaintOverlay(CPaintEvent* paintEvent)
	{
		TRACY_ZONE;

		CPainter* painter = paintEvent->painter;

		for (CBehavior* behavior : behaviors)
		{
			behavior->OnPaintOverlay(painter);
		}
	}

	CBehavior* CWidget::AddDefaultBehavior(SubClass<CBehavior> behaviorClass)
	{
		for (CBehavior* behavior : behaviors)
		{
			if (behavior->GetClass() == behaviorClass)
				return nullptr;
		}

		return CreateDefaultSubobject<CBehavior>(behaviorClass, behaviorClass->GetName().GetLastComponent());
	}

	void CWidget::HandleEvent(CEvent* event)
	{
		TRACY_ZONE;

		if (event == nullptr)
			return;
		if (!IsEnabled())
			return;

		bool popPaintCoords = false;
		bool popPaintClipRect = false;
		bool shouldPropagateDownwards = true;
		bool skipPaint = false;

		if (event->type == CEventType::FocusChanged)
		{
			CFocusEvent* focusEvent = static_cast<CFocusEvent*>(event);
			focusEvent->Consume(this);

			if (focusEvent->GotFocus() && !IsFocussed())
			{
				OnFocusGained();
				onFocused.Broadcast();
			}
			else if (focusEvent->LostFocus() && IsFocussed())
			{
				OnFocusLost();
				onUnfocused.Broadcast();
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

		if (event->type == CEventType::PaintEvent)
		{
			CPaintEvent* paintEvent = (CPaintEvent*)event;
			paintEvent->direction = CEventDirection::TopToBottom;
			CPainter* painter = paintEvent->painter;

			Vec2 scrollOffset = Vec2();

			if (parent != nullptr && (parent->allowVerticalScroll || parent->allowHorizontalScroll))
				scrollOffset = parent->normalizedScroll * (parent->contentSize - parent->GetComputedLayoutSize());

			if (paintEvent->painter != nullptr && CanPaint() && IsVisible() && IsEnabled())
			{
				paintEvent->painter->Reset();
				popPaintCoords = true;
				Vec2 origin = Vec2();
				if (parent != nullptr)
				{
					origin = parent->GetComputedLayoutTopLeft() + parent->GetFinalRootPadding().min;
				}
				paintEvent->painter->PushChildCoordinateSpace(origin - scrollOffset);

				if (clipChildren)
				{
					Vec2 extraSize = Vec2();
					if (IsWindow() && parent == nullptr)
					{
						extraSize = Vec2(rootPadding.left + rootPadding.right, rootPadding.top + rootPadding.bottom);
					}

					Vec4 borderRadius = Vec4();

					if (computedStyle.properties.KeyExists(CStylePropertyType::BorderRadius))
					{
						borderRadius = computedStyle.properties[CStylePropertyType::BorderRadius].vector;
					}

					auto contentRect = Rect::FromSize(GetComputedLayoutTopLeft(), GetComputedLayoutSize() + extraSize);
					paintEvent->painter->PushClipRect(contentRect, borderRadius);
					popPaintClipRect = true;
				}

				if (!skipPaint)
				{
					OnPaint(paintEvent);
				}

				if (!IsContainer())
				{
					shouldPropagateDownwards = false;
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

			if (mouseEvent->type == CEventType::MousePress && mouseEvent->button == MouseButton::Left)
			{
				wasClickedInside = true;
				onMouseLeftPress.Broadcast();
			}
			else if (mouseEvent->type == CEventType::MouseRelease && mouseEvent->button == MouseButton::Left && mouseEvent->isInside)
			{
				if (wasClickedInside)
				{
					onMouseLeftClick.Broadcast();
				}
				wasClickedInside = false;
			}
			
			if (event->type == CEventType::MousePress && mouseEvent->button == MouseButton::Left)
			{
				mouseEvent->Consume(this);
				stateFlags |= CStateFlag::Pressed;
				isPressed = true;
				SetNeedsStyle();
				SetNeedsPaint();
			}
			else if (event->type == CEventType::MouseRelease && mouseEvent->button == MouseButton::Left)
			{
				mouseEvent->Consume(this);
				stateFlags &= ~CStateFlag::Pressed;
				isPressed = false;
				SetNeedsStyle();
				SetNeedsPaint();
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

		// Handle behavior events
		if (event->type != CEventType::PaintEvent)
		{
			for (CBehavior* behavior : behaviors)
			{
				behavior->HandleEvent(event);
			}
		}
		
		if (event->direction == CEventDirection::TopToBottom && IsEnabled()) // Pass event down the chain
		{
			if (shouldPropagateDownwards)
			{
				for (CWidget* widget : attachedWidgets)
				{
					if (event->stopPropagation)
					{
						if (event->type == CEventType::PaintEvent && popPaintClipRect)
						{
							((CPaintEvent*)event)->painter->PopClipRect();
							popPaintClipRect = true;
						}

						// Only stop propagation to the children, but continue propagation to widgets outside this hierarchy
						event->stopPropagation = false;
						return;
					}
					widget->HandleEvent(event);
				}

				if (IsWindow())
				{
					CWindow* thisWindow = static_cast<CWindow*>(this);

					for (CWindow* attachedWindow : thisWindow->attachedWindows)
					{
						attachedWindow->HandleEvent(event);
					}
				}
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
				if (popPaintClipRect)
				{
					paintEvent->painter->PopClipRect();
					popPaintClipRect = false;
				}
				if (popPaintCoords)
				{
					paintEvent->painter->PopChildCoordinateSpace();
				}
			}
		}
	}

	CWidget* CWidget::HitTest(Vec2 windowSpaceMousePos)
	{
		if (!IsEnabled() || !IsInteractable())
			return nullptr;

		Rect rect = GetWindowSpaceRect();

		if (!rect.Contains(windowSpaceMousePos))
			return nullptr;

		for (int i = attachedWidgets.GetSize() - 1; i >= 0; --i)
		{
			CWidget* childWidget = attachedWidgets[i];

			if (childWidget == nullptr)
				continue;

			CWidget* hit = childWidget->HitTest(windowSpaceMousePos);
			if (hit != nullptr)
			{
				return hit;
			}
		}

		return this;
	}

	void CWidget::OnDestroyQueued()
	{

	}

	void CWidget::OnEnabled()
	{
		for (CWidget* widget : attachedWidgets)
		{
			widget->OnEnabled();
		}
	}

	void CWidget::OnDisabled()
	{
		if (IsFocussed())
		{
			stateFlags &= ~CStateFlag::Focused;
			SetNeedsStyle();
			SetNeedsPaint();
		}

		stateFlags &= ~(CStateFlag::Pressed | CStateFlag::Hovered);

		for (CWidget* widget : attachedWidgets)
		{
			widget->OnDisabled();
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
	}
    
} // namespace CE::Widgets

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

	void CWidget::OnSubobjectAttached(Object* object)
	{
		Super::OnSubobjectAttached(object);

		if (!object)
			return;

		// DockSpace has its own SubWidget logic
		if (object->IsOfType<CWidget>() && !IsOfType<CDockSpace>())
		{
			CWidget* widget = (CWidget*)object;
			attachedWidgets.Add(widget);
			widget->parent = this;
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

		if (object->IsOfType<CWidget>() && !IsOfType<CDockSpace>())
		{
			CWidget* widget = (CWidget*)object;
			widget->parent = nullptr;
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

	bool CWidget::IsWindow()
	{
		return IsOfType<CWindow>();
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
		needsStyle = true;

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
		return needsLayout;
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
				}
				else if (parentWindow)
				{
					window->windowSize = parentWindow->windowSize - 
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

			Vec2 pos = GetComputedLayoutTopLeft();
			Vec2 size = GetComputedLayoutSize();

			for (CWidget* widget : attachedWidgets)
			{
				widget->UpdateLayoutIfNeeded();
			}

			needsLayout = false;
		}
	}

	void CWidget::UpdateStyleIfNeeded()
	{
		if (!NeedsStyle())
			return;

		SetNeedsStyle();
		
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

		auto selectStyle = styleSheet->SelectStyle(this, stateFlags, subControl);
		computedStyle.ApplyProperties(selectStyle);

		bool layoutChanged = computedStyle.CompareLayoutProperties(prevComputedStyle);

		if (layoutChanged)
		{
			CE_LOG(Info, All, "Layout changed!");
		}

		for (const auto& [property, value] : selectStyle.properties)
		{
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

		needsStyle = false;

		for (CWidget* child : attachedWidgets)
		{
			child->UpdateStyleIfNeeded();
		}

		SetNeedsLayout();
	}

	void CWidget::AddSubWidget(CWidget* widget)
	{
		AttachSubobject(widget);
	}

	void CWidget::RemoveSubWidget(CWidget* widget)
	{
		DetachSubobject(widget);
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

		if (computedStyle.properties.KeyExists(CStylePropertyType::Background))
		{
			Color bgColor = computedStyle.properties[CStylePropertyType::Background].color;
			
		}
	}

	void CWidget::HandleEvent(CEvent* event)
	{
		if (event == nullptr)
			return;

		bool popPaintCoords = false;

		// Handle event for this widget
		if (event->type == CEventType::PaintEvent)
		{
			CPaintEvent* paintEvent = (CPaintEvent*)event;
			paintEvent->direction = CEventDirection::TopToBottom;

			if (paintEvent->painter != nullptr && CanPaint())
			{
				paintEvent->painter->Reset();
				popPaintCoords = true;
				auto origin = GetComputedLayoutTopLeft();
				if (parent != nullptr)
					origin += parent->rootPadding.min;
				paintEvent->painter->PushChildCoordinateSpace(origin);
				OnPaint(paintEvent);
			}
		}
		
		if (event->direction == CEventDirection::TopToBottom) // Pass event down the chain
		{
			for (CWidget* widget : attachedWidgets)
			{
				if (event->isConsumed && event->stopPropagation)
					return;
				widget->HandleEvent(event);
			}
		}
		else if (event->direction == CEventDirection::BottomToTop) // Pass event up the chain
		{
			for (CWidget* parent = this->parent; parent != nullptr; parent = parent->parent)
			{
				if (parent != this) // Prevent infinite recursion!
				{
					parent->HandleEvent(event);
				}
			}
		}

		if (event->type == CEventType::PaintEvent)
		{
			CPaintEvent* paintEvent = (CPaintEvent*)event;
			if (paintEvent->painter != nullptr && popPaintCoords)
			{
				paintEvent->painter->PopChildCoordinateSpace();
			}
		}
	}
    
} // namespace CE::Widgets

#include "CrystalWidgets.h"

namespace CE::Widgets
{

	CWidget::CWidget()
	{
		node = YGNodeNew();
		YGNodeSetContext(node, this);

		this->styleSheet = CreateDefaultSubobject<CSSStyleSheet>("StyleSheet");
	}

	CWidget::~CWidget()
	{
		YGNodeFree(node);
		node = nullptr;
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

		if (object->IsOfType<CWidget>())
		{
			CWidget* widget = (CWidget*)object;
			widget->ownerWindow = ownerWindow;
		}
	}

	void CWidget::OnSubobjectDetached(Object* object)
	{
		Super::OnSubobjectDetached(object);

		if (!object)
			return;

		if (object->IsOfType<CWidget>())
		{
			CWidget* widget = (CWidget*)object;
			widget->ownerWindow = nullptr;
		}
	}

	void CWidget::SetNeedsLayout()
	{
		needsLayout = true;

		for (int i = 0; i < attachedWidgets.GetSize(); ++i)
		{
			attachedWidgets[i]->SetNeedsLayout();
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
		if (needsStyle)
			return true;

		for (auto widget : attachedWidgets)
		{
			if (widget != this && widget != nullptr && widget->NeedsStyle())
				return true;
		}

		return false;
	}

	void CWidget::UpdateStyleIfNeeded()
	{
		if (!NeedsStyle())
			return;

		YGNodeReset(node);
		computedStyle = {};

		auto selectStyle = styleSheet->SelectStyle(this, stateFlags, subControl);
		computedStyle.ApplyProperties(selectStyle);

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

		SetNeedsLayout();
	}

	void CWidget::AddSubWidget(CWidget* widget)
	{
		AttachSubobject(widget);

		attachedWidgets.Add(widget);
		widget->parent = this;

		SetNeedsPaint();
	}

	void CWidget::RemoveSubWidget(CWidget* widget)
	{
		DetachSubobject(widget);

		attachedWidgets.Remove(widget);
		widget->parent = nullptr;

		SetNeedsPaint();
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
		
	}

	void CWidget::HandleEvent(CEvent* event)
	{
		if (event == nullptr)
			return;

		// Handle event for this widget
		if (event->type == CEventType::PaintEvent)
		{
			CPaintEvent* paintEvent = (CPaintEvent*)event;
			if (paintEvent->painter != nullptr && CanPaint())
			{
				paintEvent->painter->Reset();
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
	}
    
} // namespace CE::Widgets

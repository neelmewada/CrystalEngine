#include "FusionCore.h"

namespace CE
{

	FStackBox::FStackBox()
	{
		
	}

	FStackBox::~FStackBox()
	{
		
	}

	void FStackBox::CalculateIntrinsicSize()
	{
		ZoneScoped;

		if (children.IsEmpty())
		{
			Super::CalculateIntrinsicSize();
			return;
		}

		intrinsicSize.width = m_Padding.left + m_Padding.right;
		intrinsicSize.height = m_Padding.top + m_Padding.bottom;

		Vec2 contentSize = {};

		for (FWidget* child : children)
		{
			child->CalculateIntrinsicSize();

			Vec2 childSize = child->GetIntrinsicSize();
			Vec4 childMargin = child->GetMargin();

			if (m_Direction == FStackBoxDirection::Horizontal)
			{
				contentSize.width += childSize.width;
				contentSize.height = Math::Max(contentSize.height, childSize.height + childMargin.top + childMargin.bottom);
			}
			else if (m_Direction == FStackBoxDirection::Vertical)
			{
				contentSize.height += childSize.height;
				contentSize.width = Math::Max(contentSize.width, childSize.width + childMargin.left + childMargin.right);
			}
		}

		intrinsicSize.width += contentSize.width;
		intrinsicSize.height += contentSize.height;

		intrinsicSize.width = Math::Clamp(intrinsicSize.width, 
			m_MinWidth + m_Padding.left + m_Padding.right, 
			m_MaxWidth + m_Padding.left + m_Padding.right);

		intrinsicSize.height = Math::Clamp(intrinsicSize.height, 
			m_MinHeight + m_Padding.top + m_Padding.bottom,
			m_MaxHeight + m_Padding.top + m_Padding.bottom);
	}

	void FStackBox::PlaceSubWidgets()
	{
		ZoneScoped;

		Super::PlaceSubWidgets();

		if (children.IsEmpty())
		{
			return;
		}

		bool isTextStack = GetName() == "TextInputHStack";

		Vec2 curPos = Vec2(m_Padding.left, m_Padding.top);
		f32 crossAxisSize = 0;
		f32 remainingSize = 0;
		Vec2 availableSize = computedSize - Vec2(m_Padding.left + m_Padding.right,
			m_Padding.top + m_Padding.bottom);

		if (m_Direction == FStackBoxDirection::Horizontal)
		{
			crossAxisSize = availableSize.height;
			remainingSize = availableSize.width;
		}
		else if (m_Direction == FStackBoxDirection::Vertical)
		{
			crossAxisSize = availableSize.width;
			remainingSize = availableSize.height;
		}

		f32 totalFillRatio = 0.0f;

		for (FWidget* child : children)
		{
			Vec2 childIntrinsicSize = child->GetIntrinsicSize();
			
			if (m_Direction == FStackBoxDirection::Horizontal)
			{
				if (child->m_FillRatio > 0)
				{
					totalFillRatio += child->m_FillRatio;
				}

				remainingSize -= childIntrinsicSize.width + child->GetMargin().left + child->GetMargin().right;
			}
			else if (m_Direction == FStackBoxDirection::Vertical)
			{
				if (child->m_FillRatio > 0)
				{
					totalFillRatio += child->m_FillRatio;
				}

				remainingSize -= childIntrinsicSize.height + child->GetMargin().top + child->GetMargin().bottom;
			}
		}

		//remainingSize = Math::Max(remainingSize, 0.0f);

		for (FWidget* child : children)
		{
			Vec2 childIntrinsicSize = child->GetIntrinsicSize();

			child->computedPosition = curPos + Vec2(child->m_Margin.left, child->m_Margin.top);

			CE::VAlign vertAlign = child->m_VAlign;
			if (vertAlign == CE::VAlign::Auto)
				vertAlign = m_ContentVAlign;

			CE::HAlign horiAlign = child->m_HAlign;
			if (horiAlign == HAlign::Auto)
				horiAlign = m_ContentHAlign;

			if (m_Direction == FStackBoxDirection::Horizontal)
			{
				switch (vertAlign)
				{
				case VAlign::Auto:
				case VAlign::Fill:
					child->computedSize.height = availableSize.height;
					break;
				case VAlign::Top:
					child->computedSize.height = Math::Min(childIntrinsicSize.height, availableSize.height);
					break;
				case VAlign::Center:
					child->computedSize.height = Math::Min(childIntrinsicSize.height, availableSize.height);
					child->computedPosition.y = child->computedPosition.y + (availableSize.height - childIntrinsicSize.height) * 0.5f;
					break;
				case VAlign::Bottom:
					child->computedSize.height = Math::Min(childIntrinsicSize.height, availableSize.height);
					child->computedPosition.y = child->computedPosition.y + (availableSize.height - childIntrinsicSize.height);
					break;
				}

				if (child->m_FillRatio > 0)
				{
					child->computedSize.width = childIntrinsicSize.width + remainingSize * child->m_FillRatio / totalFillRatio;
				}
				else
				{
					child->computedSize.width = childIntrinsicSize.width;
				}

				child->ApplySizeConstraints();

				curPos.x += child->computedSize.width + child->m_Margin.right;
			}
			else if (m_Direction == FStackBoxDirection::Vertical)
			{
				switch (horiAlign)
				{
				case HAlign::Auto:
				case HAlign::Fill:
					child->computedSize.width = availableSize.width;
					break;
				case HAlign::Left:
					child->computedSize.width = Math::Min(childIntrinsicSize.width, availableSize.width);
					break;
				case HAlign::Center:
					child->computedSize.width = Math::Min(childIntrinsicSize.width, availableSize.width);
					child->computedPosition.x = curPos.x + (availableSize.width - childIntrinsicSize.width) * 0.5f;
					break;
				case HAlign::Right:
					child->computedSize.width = Math::Min(childIntrinsicSize.width, availableSize.width);
					child->computedPosition.x = curPos.x + (availableSize.width - childIntrinsicSize.width);
					break;
				}

				if (child->m_FillRatio > 0)
				{
					child->computedSize.height = childIntrinsicSize.height + remainingSize * child->m_FillRatio / totalFillRatio;
				}
				else
				{
					child->computedSize.height = childIntrinsicSize.height;
				}

				child->ApplySizeConstraints();

				curPos.y += child->computedSize.height + child->m_Margin.bottom;
			}
			
			child->PlaceSubWidgets();
		}
	}

	void FStackBox::OnPaint(FPainter* painter)
	{
		Super::OnPaint(painter);

		if (children.IsEmpty())
			return;

		painter->PushChildCoordinateSpace(localTransform);

		for (FWidget* child : children)
		{
			child->OnPaint(painter);
		}

		painter->PopChildCoordinateSpace();
	}

	void FStackBox::Construct()
	{
		
	}

}

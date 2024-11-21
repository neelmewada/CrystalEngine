#include "FusionCore.h"

namespace CE
{

	FStackBox::FStackBox()
	{
		m_Direction = FStackBoxDirection::Horizontal;
		m_HAlign = HAlign::Fill;
		m_VAlign = VAlign::Fill;
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

		for (const auto& child : children)
		{
			if (!child->Enabled())
				continue;

			child->CalculateIntrinsicSize();

			Vec2 childSize = child->GetIntrinsicSize();
			Vec4 childMargin = child->Margin();

			if (m_Direction == FStackBoxDirection::Horizontal)
			{
				contentSize.width += childSize.width + childMargin.left;
				contentSize.height = Math::Max(contentSize.height, childSize.height + childMargin.top + childMargin.bottom);
			}
			else if (m_Direction == FStackBoxDirection::Vertical)
			{
				contentSize.height += childSize.height + childMargin.top;
				contentSize.width = Math::Max(contentSize.width, childSize.width + childMargin.left + childMargin.right);
			}
		}

		if (m_Direction == FStackBoxDirection::Horizontal)
		{
			contentSize.width += m_Gap * (children.GetSize() - 1);
		}
		else if (m_Direction == FStackBoxDirection::Vertical)
		{
			contentSize.height += m_Gap * (children.GetSize() - 1);
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

		Vec2 curPos = Vec2(m_Padding.left, m_Padding.top);
		f32 crossAxisSize = 0;
		f32 remainingSize = 0;
		Vec2 availableSize = computedSize - Vec2(m_Padding.left + m_Padding.right,
			m_Padding.top + m_Padding.bottom);

		if (m_Direction == FStackBoxDirection::Horizontal)
		{
			availableSize.width -= m_Gap * (children.GetSize() - 1);

			crossAxisSize = availableSize.height;
			remainingSize = availableSize.width;
		}
		else if (m_Direction == FStackBoxDirection::Vertical)
		{
			availableSize.height -= m_Gap * (children.GetSize() - 1);

			crossAxisSize = availableSize.width;
			remainingSize = availableSize.height;
		}

		f32 totalFillRatio = 0.0f;

		for (const auto& child : children)
		{
			if (!child->Enabled())
				continue;

			Vec2 childIntrinsicSize = child->GetIntrinsicSize();
			
			if (m_Direction == FStackBoxDirection::Horizontal)
			{
				if (child->m_FillRatio > 0)
				{
					totalFillRatio += child->m_FillRatio;
					remainingSize -= child->Margin().left + child->Margin().right +
						child->Padding().left + child->Padding().right + child->MinWidth();
				}
				else
				{
					remainingSize -= childIntrinsicSize.width + child->Margin().left + child->Margin().right;
				}
			}
			else if (m_Direction == FStackBoxDirection::Vertical)
			{
				if (child->m_FillRatio > 0)
				{
					totalFillRatio += child->m_FillRatio;
					remainingSize -= child->Margin().top + child->Margin().bottom +
						child->Padding().top + child->Padding().bottom + child->MinHeight();
				}
				else
				{
					remainingSize -= childIntrinsicSize.height + child->Margin().top + child->Margin().bottom;
				}
			}
		}

		//remainingSize = Math::Max(remainingSize, 0.0f);

		for (const auto& child : children)
		{
			if (!child->Enabled())
				continue;

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
					child->computedSize.width = child->Margin().left + child->Margin().right +
						child->Padding().left + child->Padding().right + child->MinWidth() + 
						remainingSize * child->m_FillRatio / totalFillRatio;
				}
				else
				{
					child->computedSize.width = childIntrinsicSize.width;
				}

				child->ApplySizeConstraints();

				curPos.x += child->computedSize.width + child->m_Margin.left + child->m_Margin.right + m_Gap;
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
					child->computedSize.height = child->Margin().top + child->Margin().bottom +
						child->Padding().top + child->Padding().bottom + child->MinHeight() +
						remainingSize * child->m_FillRatio / totalFillRatio;
					//child->computedSize.height = childIntrinsicSize.height + remainingSize * child->m_FillRatio / totalFillRatio;
				}
				else
				{
					child->computedSize.height = childIntrinsicSize.height;
				}

				child->ApplySizeConstraints();

				curPos.y += child->computedSize.height + child->m_Margin.top + child->m_Margin.bottom + m_Gap;
			}
			
			child->PlaceSubWidgets();
		}
	}

	void FStackBox::OnPaint(FPainter* painter)
	{
		Super::OnPaint(painter);

		if (children.IsEmpty() || !Enabled())
			return;

		painter->PushChildCoordinateSpace(localTransform);
		if (m_ClipChildren)
		{
			painter->PushClipShape(Matrix4x4::Identity(), computedSize);
		}

		for (const auto& child : children)
		{
			if (!child->Enabled() || !child->Visible())
				continue;

			child->OnPaint(painter);
		}

		if (m_ClipChildren)
		{
			painter->PopClipShape();
		}
		painter->PopChildCoordinateSpace();
	}

	void FStackBox::Construct()
	{
		
	}

}

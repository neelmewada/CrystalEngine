#include "FusionCore.h"

namespace CE
{

    FSplitBox::FSplitBox()
    {
		m_SplitterSize = 2.5f;
    }

    void FSplitBox::CalculateIntrinsicSize()
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
			if (!child->Enabled())
				continue;

			child->CalculateIntrinsicSize();

			Vec2 childSize = child->GetIntrinsicSize();
			Vec4 childMargin = child->Margin();

			if (m_Direction == FSplitDirection::Horizontal)
			{
				contentSize.width += childSize.width;
				contentSize.height = Math::Max(contentSize.height, childSize.height + childMargin.top + childMargin.bottom);
			}
			else if (m_Direction == FSplitDirection::Vertical)
			{
				contentSize.height += childSize.height;
				contentSize.width = Math::Max(contentSize.width, childSize.width + childMargin.left + childMargin.right);
			}
		}

		if (m_Direction == FSplitDirection::Horizontal)
		{
			contentSize.width += m_SplitterSize * children.GetSize();
		}
		else if (m_Direction == FSplitDirection::Vertical)
		{
			contentSize.height += m_SplitterSize * children.GetSize();
		}

		intrinsicSize.width += contentSize.width;
		intrinsicSize.height += contentSize.height;

		ApplyIntrinsicSizeConstraints();
    }

    void FSplitBox::PlaceSubWidgets()
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

		if (m_Direction == FSplitDirection::Horizontal)
		{
			crossAxisSize = availableSize.height;
			remainingSize = availableSize.width;
		}
		else if (m_Direction == FSplitDirection::Vertical)
		{
			crossAxisSize = availableSize.width;
			remainingSize = availableSize.height;
		}

		remainingSize -= m_SplitterSize * children.GetSize();

		for (FWidget* child : children)
		{
			if (!child->Enabled())
				continue;

			Vec2 childIntrinsicSize = child->GetIntrinsicSize();

			child->SetComputedPosition(curPos + Vec2(child->Margin().left, child->Margin().top));

			CE::VAlign vertAlign = child->VAlign();
			if (vertAlign == CE::VAlign::Auto)
				vertAlign = m_ContentVAlign;

			CE::HAlign horiAlign = child->HAlign();
			if (horiAlign == HAlign::Auto)
				horiAlign = m_ContentHAlign;

			f32 fillRatio = child->FillRatio();

			if (m_Direction == FSplitDirection::Horizontal)
			{
				f32 minWidth = (child->MinWidth() + child->Padding().left + child->Padding().right);
				fillRatio = Math::Max(fillRatio, minWidth / remainingSize);

				child->computedSize.width = remainingSize * fillRatio;
				child->computedSize.height = availableSize.height;

				curPos.x += child->computedSize.width + m_SplitterSize;
			}
			else if (m_Direction == FSplitDirection::Vertical)
			{
				f32 minHeight = (child->MinHeight() + child->Padding().top + child->Padding().bottom);
				fillRatio = Math::Max(fillRatio, minHeight / remainingSize);

				child->computedSize.height = remainingSize * fillRatio;
				child->computedSize.width = availableSize.width;

				curPos.y += child->computedSize.height + m_SplitterSize;
			}

			child->PlaceSubWidgets();
		}
    }

    void FSplitBox::Construct()
    {
        Super::Construct();
        
    }

    void FSplitBox::OnPaint(FPainter* painter)
    {
		Super::OnPaint(painter);

		if (children.IsEmpty() || !Enabled())
			return;

		painter->PushChildCoordinateSpace(localTransform);
		if (m_ClipChildren)
		{
			painter->PushClipShape(Matrix4x4::Identity(), computedSize);
		}

		for (FWidget* child : children)
		{
			if (!child->Enabled())
				continue;

			child->OnPaint(painter);
		}

		if (m_ClipChildren)
		{
			painter->PopClipShape();
		}
		painter->PopChildCoordinateSpace();
    }

}


#include "FusionCore.h"

namespace CE
{

    FScrollBox::FScrollBox()
    {
        m_VerticalScroll = true;
        m_HorizontalScroll = false;

        m_ScrollBarShape = FRoundedRectangle(5);
        m_ScrollBarWidth = 3;
    }

    void FScrollBox::CalculateIntrinsicSize()
    {
        if (!GetChild())
        {
            Super::CalculateIntrinsicSize();
            return;
        }

        FWidget* child = GetChild();

        child->CalculateIntrinsicSize();

        intrinsicSize = child->GetIntrinsicSize() + Vec2(m_Padding.left + m_Padding.right, m_Padding.top + m_Padding.bottom);
    
        ApplyIntrinsicSizeConstraints();
    }

    void FScrollBox::PlaceSubWidgets()
    {
        Super::PlaceSubWidgets();
    }

    FWidget* FScrollBox::HitTest(Vec2 mousePosition)
    {
        return Super::HitTest(mousePosition);
    }

    
    
}


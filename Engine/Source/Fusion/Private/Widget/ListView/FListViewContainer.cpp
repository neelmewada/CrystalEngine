#include "Fusion.h"

namespace CE
{

    FListViewContainer::FListViewContainer()
    {

    }

    FListViewContainer::~FListViewContainer()
    {
    }

    void FListViewContainer::CalculateIntrinsicSize()
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


        }
    }

    void FListViewContainer::PlaceSubWidgets()
    {
	    Super::PlaceSubWidgets();

    }

}


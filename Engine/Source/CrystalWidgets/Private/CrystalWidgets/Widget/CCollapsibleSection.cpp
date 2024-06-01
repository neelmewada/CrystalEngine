#include "CrystalWidgets.h"

namespace CE::Widgets
{

    CCollapsibleSection::CCollapsibleSection()
    {
        
    }

    CCollapsibleSection::~CCollapsibleSection()
    {
	    
    }

    void CCollapsibleSection::SetExpanded(bool expanded)
    {
        if (isExpanded == expanded)
            return;

        isExpanded = expanded;
        content->SetEnabled(isExpanded);

        dropDownArrow->SetRotation(isExpanded ? 90 : 0);

        if (isExpanded)
        {
            stateFlags &= ~CStateFlag::Collapsed;
        }
        else
        {
            stateFlags |= CStateFlag::Collapsed;
        }

        SetNeedsStyle();
        SetNeedsLayout();
        SetNeedsPaint();
    }

    void CCollapsibleSection::SetTitle(const String& title)
    {
        titleLabel->SetText(title);
    }

    void CCollapsibleSection::Construct()
    {
	    Super::Construct();

        header = CreateObject<CWidget>(this, "SectionHeader");
        header->SetClipChildren(true);

	    {
            dropDownArrow = CreateObject<CImage>(header, "SectionArrow");
            dropDownArrow->SetBackgroundImage("/Editor/Assets/Icons/Arrow");

            titleLabel = CreateObject<CLabel>(header, "SectionTitle");
            titleLabel->SetText("Title");
	    }

        dropDownArrow->onMouseLeftPress.Bind()

        Bind(dropDownArrow, MEMBER_FUNCTION(CWidget, OnMouseLeftPress), [this]
            {
                SetExpanded(!isExpanded);
            });

        Bind(header, MEMBER_FUNCTION(CWidget, OnMouseLeftPress), [this]
            {
                SetExpanded(!isExpanded);
            });
        
        content = CreateObject<CWidget>(this, "SectionContent");
        content->SetEnabled(isInitiallyExpanded);
        content->SetClipChildren(true);

        isExpanded = isInitiallyExpanded;

        if (isExpanded)
        {
            stateFlags &= ~CStateFlag::Collapsed;
        }
        else
        {
            stateFlags |= CStateFlag::Collapsed;
        }
    }

} // namespace CE::Widgets

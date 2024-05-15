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
	    {
            dropDownArrow = CreateObject<CImage>(header, "SectionArrow");
            dropDownArrow->SetBackgroundImage("/Editor/Assets/Icons/Arrow");

            titleLabel = CreateObject<CLabel>(header, "SectionTitle");
            titleLabel->SetText("Title");
	    }

        Bind(header, MEMBER_FUNCTION(CWidget, OnMouseLeftPress), [this]
            {
                SetExpanded(!isExpanded);
            });
        
        content = CreateObject<CWidget>(this, "SectionContent");
        content->SetEnabled(isInitiallyExpanded);

        isExpanded = isInitiallyExpanded;
    }

} // namespace CE::Widgets

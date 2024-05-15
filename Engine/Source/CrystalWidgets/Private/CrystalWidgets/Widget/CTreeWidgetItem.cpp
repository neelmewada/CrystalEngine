#include "CrystalWidgets.h"

namespace CE::Widgets
{

    CTreeWidgetItem::CTreeWidgetItem()
    {
        allowVerticalScroll = false;
        allowHorizontalScroll = true;
        createLabel = true;
        isExpanded = true;
        clipChildren = false;

        arrowIcon = CreateDefaultSubobject<CImage>("ArrowButton");
    }

    CTreeWidgetItem::~CTreeWidgetItem()
    {
	    
    }

    bool CTreeWidgetItem::IsSubWidgetAllowed(Class* subWidgetClass)
    {
	    return Super::IsSubWidgetAllowed(subWidgetClass);
    }

    bool CTreeWidgetItem::IsExpandedInHierarchy() const
    {
        if (isExpanded)
            return true;
        if (parent == nullptr || !parent->IsOfType<CTreeWidgetItem>())
            return true;

        return ((CTreeWidgetItem*)parent)->IsExpandedInHierarchy();
    }

    void CTreeWidgetItem::SetExpanded(bool expanded)
    {
        if (isExpanded == expanded)
            return;

        isExpanded = expanded;
        arrowIcon->rotation = isExpanded ? 90 : 0;

        GetTreeWidget()->UpdateRows();

        SetNeedsLayout();
        SetNeedsPaint();
    }

    CTreeWidget* CTreeWidgetItem::GetTreeWidget()
    {
        if (treeWidget)
            return treeWidget;

        std::function<CTreeWidget* (CWidget*)> visitor = [&](CWidget* widget) -> CTreeWidget*
            {
                if (widget == nullptr)
                    return nullptr;

                if (widget->IsOfType<CTreeWidget>())
                    return static_cast<CTreeWidget*>(widget);

                return visitor(widget->parent);
            };

        treeWidget = visitor(parent);

        return treeWidget;
    }

    const String& CTreeWidgetItem::GetText() const
    {
        static const String empty{};
        if (!label)
            return empty;

        return label->GetText();
    }

    void CTreeWidgetItem::SetText(const String& text)
    {
        if (label)
            label->SetText(text);
    }

    void CTreeWidgetItem::AddChild(CTreeWidgetItem* child)
    {
        children.Add(child);
    }

    void CTreeWidgetItem::SetForceExpanded(bool forceExpanded)
    {
        this->forceExpanded = forceExpanded;

        arrowIcon->SetEnabled(!forceExpanded);

        SetNeedsLayout();
        SetNeedsPaint();
    }

    void CTreeWidgetItem::Construct()
    {
	    Super::Construct();

        arrowIcon->SetBackgroundImage("/Editor/Assets/Icons/Arrow");

        Bind(arrowIcon, MEMBER_FUNCTION(CImage, OnMouseLeftPress), [&]
            {
                if (arrowIcon->IsVisible())
                {
                    SetExpanded(!isExpanded);
                }
            });

        if (createLabel)
	    {
		    label = CreateObject<CLabel>(this, "Title");
	    	label->SetText("");
	    }

        arrowIcon->rotation = isExpanded ? 90 : 0;
    }

    void CTreeWidgetItem::OnSubobjectAttached(Object* subobject)
    {
        Super::OnSubobjectAttached(subobject);

        if (subobject->IsOfType<CTreeWidgetItem>())
        {
            CTreeWidgetItem* treeItem = static_cast<CTreeWidgetItem*>(subobject);
            treeItem->treeWidget = treeWidget;

            children.Add(treeItem);

            GetTreeWidget()->UpdateRows();
        }
    }

    void CTreeWidgetItem::OnSubobjectDetached(Object* subobject)
    {
	    Super::OnSubobjectDetached(subobject);

        if (subobject->IsOfType<CTreeWidgetItem>())
        {
            CTreeWidgetItem* treeItem = static_cast<CTreeWidgetItem*>(subobject);
            //treeItem->treeWidget = nullptr;

            //children.Remove(treeItem);
        }
    }

    void CTreeWidgetItem::OnPaint(CPaintEvent* paintEvent)
    {
        Super::OnPaint(paintEvent);

        CPainter* painter = paintEvent->painter;

        Vec2 size = GetComputedLayoutSize();

        Vec2 regionSize = GetTreeWidget()->GetComputedLayoutSize();

    }

} // namespace CE::Widgets

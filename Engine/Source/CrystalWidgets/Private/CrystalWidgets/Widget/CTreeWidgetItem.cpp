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
    }

    CTreeWidgetItem::~CTreeWidgetItem()
    {
	    
    }

    bool CTreeWidgetItem::IsSubWidgetAllowed(Class* subWidgetClass)
    {
	    return Super::IsSubWidgetAllowed(subWidgetClass);
    }

    void CTreeWidgetItem::SetExpanded(bool expanded)
    {
        if (isExpanded == expanded)
            return;

        isExpanded = expanded;

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

    void CTreeWidgetItem::Construct()
    {
	    Super::Construct();

        if (createLabel)
	    {
		    label = CreateObject<CLabel>(this, "Title");
	    	label->SetText("");
	    }

    }

    void CTreeWidgetItem::OnSubobjectAttached(Object* subobject)
    {
        Super::OnSubobjectAttached(subobject);

        if (subobject->IsOfType<CTreeWidgetItem>())
        {
            CTreeWidgetItem* treeItem = static_cast<CTreeWidgetItem*>(subobject);
            treeItem->treeWidget = treeWidget;

            children.Add(treeItem);
        }
    }

    void CTreeWidgetItem::OnSubobjectDetached(Object* subobject)
    {
	    Super::OnSubobjectDetached(subobject);

        if (subobject->IsOfType<CTreeWidgetItem>())
        {
            CTreeWidgetItem* treeItem = static_cast<CTreeWidgetItem*>(subobject);
            treeItem->treeWidget = nullptr;

            children.Remove(treeItem);
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

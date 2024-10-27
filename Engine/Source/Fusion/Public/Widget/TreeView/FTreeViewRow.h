#pragma once

namespace CE
{
    CLASS()
    class FUSION_API FTreeViewRow : public FStyledWidget
    {
        CE_CLASS(FTreeViewRow, FStyledWidget)
    public:

        // - Public API -

        u32 GetCellCount() const { return contentStack->GetChildCount(); }

        FTreeViewCell* GetCell(u32 index) const;

        void AddCell(FTreeViewCell& cell);
        void AddCell(FTreeViewCell* cell);

    protected:

        FTreeViewRow();

        void Construct() override;

        FHorizontalStack* contentStack = nullptr;

        FModelIndex index{};
        bool isAlternate = false;
        FTreeView* treeView = nullptr;

    public: // - Fusion Properties -

        template<typename... TWidget> requires TMatchAllBaseClass<FWidget, TWidget...>::Value
        Self& Cells(TWidget&... widgets)
        {
            contentStack->DestroyAllChildren();

            std::initializer_list<FWidget*> list = { &widgets... };
            int size = list.size();
            int i = -1;

            for (FWidget* widget : list)
            {
                i++;
                if (!widget->IsOfType<FTreeViewCell>())
                {
                    CE_LOG(Error, All, "Invalid widget of type {}! Expected widget FTreeViewCell.", widget->GetClass()->GetName().GetLastComponent());
                    continue;
                }
                contentStack->AddChild(widget);
            }

            return *this;
        }

        FUSION_WIDGET;
        friend class FTreeView;
        friend class FTreeViewContainer;
    };
    
}

#include "FTreeViewRow.rtti.h"

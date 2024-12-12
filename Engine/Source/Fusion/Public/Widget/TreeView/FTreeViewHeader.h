#pragma once

namespace CE
{
    class FTreeView;


    CLASS()
    class FUSION_API FTreeViewHeader : public FCompoundWidget
    {
        CE_CLASS(FTreeViewHeader, FCompoundWidget)
    public:

        // - Public API -

        u32 GetColumnCount() const;

        FTreeViewHeaderColumn* GetColumn(u32 index);

    protected:

        FTreeViewHeader();

        void Construct() override;

        void HandleEvent(FEvent* event) override;

        FStyledWidget* CreateSeparator();

        FSplitBox* contentStack = nullptr;
        Array<FStyledWidget*> separators;
        FTreeView* treeView = nullptr;

    public: // - Fusion Properties - 

        template<typename... TWidget> requires TMatchAllBaseClass<FWidget, TWidget...>::Value
        Self& Columns(TWidget&... widgets)
        {
            separators.Clear();
            contentStack->DestroyAllChildren();

            std::initializer_list<FWidget*> list = { &widgets... };
            int size = list.size();
            int i = -1;

            for (FWidget* widget : list)
            {
                i++;
                if (!widget->IsOfType<FTreeViewHeaderColumn>())
                {
                    CE_LOG(Error, All, "Invalid widget of type {}! Expected widget FTreeViewHeaderColumn.", widget->GetClass()->GetName().GetLastComponent());
	                continue;
                }
                contentStack->AddChild(widget);
                if (i < size - 1)
                {
                    //separators.Add(CreateSeparator());
                    //contentStack->AddChild(separators.Top());
                }
            }

            return *this;
        }

        FUSION_WIDGET;
        friend class FTreeView;
        friend class FTreeViewStyle;
    };
    
}

#include "FTreeViewHeader.rtti.h"

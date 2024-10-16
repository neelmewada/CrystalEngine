#pragma once

namespace CE
{
    CLASS()
    class FUSION_API FTreeViewRow : public FStyledWidget
    {
        CE_CLASS(FTreeViewRow, FStyledWidget)
    public:

        // - Public API -

    protected:

        FTreeViewRow();

        void Construct() override;

        FHorizontalStack* contentStack = nullptr;

    public: // - Fusion Properties - 

        template<typename... TWidget> requires TMatchAllBaseClass<FWidget, TWidget...>::Value
        Self& Columns(TWidget&... widgets)
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
    };
    
}

#include "FTreeViewRow.rtti.h"

#pragma once

namespace CE
{
    class FTreeViewHeader;
    class FTreeViewRow;
    class FTreeViewModel;

    CLASS(Abstract)
    class FUSION_API FTreeViewModel : public FAbstractItemModel
    {
        CE_CLASS(FTreeViewModel, FAbstractItemModel)
    protected:

        FTreeViewModel();

    public:

        virtual ~FTreeViewModel();

        virtual void SetData(u32 row, FTreeViewRow& rowWidget, const FModelIndex& parent = {}) = 0;

        virtual void SetHeaderData(FTreeViewHeader& header) {}

        friend class FTreeView;
    };

    
} // namespace CE

#include "FTreeViewModel.rtti.h"

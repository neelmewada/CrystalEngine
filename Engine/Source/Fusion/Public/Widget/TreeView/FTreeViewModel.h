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

        friend class FTreeView;
    };

    
} // namespace CE

#include "FTreeViewModel.rtti.h"

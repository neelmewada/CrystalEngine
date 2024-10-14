#pragma once

namespace CE
{
    CLASS(Abstract)
    class FUSION_API FTreeViewModel : public Object
    {
        CE_CLASS(FTreeViewModel, Object)
    protected:

        FTreeViewModel();

    public:

        virtual ~FTreeViewModel();

        virtual void SetData(FTreeViewRow& row) = 0;

        friend class FTreeView;
    };
    
} // namespace CE

#include "FTreeViewModel.rtti.h"

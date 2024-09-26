#pragma once

namespace CE
{
    class FListView;
    class FListItem;

    CLASS()
    class FUSION_API FListViewModel : public FDataModel
    {
        CE_CLASS(FListViewModel, FDataModel)
    public:

        FListViewModel();

        virtual ~FListViewModel();

    protected:

        void OnModelPropertyEdited(const CE::Name& propertyName, Object* modifyingObject) override;

    public:

        MODEL_PROPERTY(Array<FListItem*>, ItemList);

    };
    
} // namespace CE

#include "FListViewModel.rtti.h"

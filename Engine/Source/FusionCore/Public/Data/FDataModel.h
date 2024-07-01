#pragma once

namespace CE
{
    class FWidget;

    CLASS()
    class FUSIONCORE_API FDataModel : public Object
    {
        CE_CLASS(FDataModel, Object)
    public:

        FDataModel();

    protected:

        virtual void OnDataPropertyModified(const Name& propertyName);


        friend class FWidget;
    };
    
} // namespace CE

#include "FDataModel.rtti.h"
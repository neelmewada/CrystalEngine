#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FDataModel : public Object
    {
        CE_CLASS(FDataModel, Object)
    public:

        FDataModel();

    protected:

        virtual void OnDataPropertyModified(const Name& propertyName);

    };
    
} // namespace CE

#include "FDataModel.rtti.h"
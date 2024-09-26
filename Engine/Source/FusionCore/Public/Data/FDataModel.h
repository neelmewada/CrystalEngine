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

        virtual void OnModelPropertyEdited(const CE::Name& propertyName, Object* modifyingObject) {}

    protected:


        friend class FWidget;
    };
    
} // namespace CE

#include "FDataModel.rtti.h"
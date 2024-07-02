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

        virtual void OnModelPropertyModified(const CE::Name& propertyName) {}
        virtual void OnModelPropertyEdited(const CE::Name& propertyName) {}

    protected:


        friend class FWidget;
    };
    
} // namespace CE

#include "FDataModel.rtti.h"
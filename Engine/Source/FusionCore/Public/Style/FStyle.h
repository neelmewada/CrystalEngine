#pragma once

namespace CE
{
    class FWidget;
    class FStyledWidget;

    CLASS(Abstract)
    class FUSIONCORE_API FStyle : public Object
    {
        CE_CLASS(FStyle, Object)
    public:

        FStyle();
        virtual ~FStyle();

        virtual SubClass<FWidget> GetWidgetClass() const;

        virtual void MakeStyle(FWidget& widget) = 0;

    };
    
} // namespace CE

#include "FStyle.rtti.h"

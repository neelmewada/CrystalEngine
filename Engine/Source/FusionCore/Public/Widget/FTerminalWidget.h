#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FTerminalWidget : public FWidget
    {
        CE_CLASS(FTerminalWidget, FWidget)
    public:

        FTerminalWidget();

    protected:

        void Construct() override final;

    protected: // - Fusion Fields -


    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "FTerminalWidget.rtti.h"

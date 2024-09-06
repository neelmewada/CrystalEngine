#pragma once

namespace CE
{
    CLASS()
    class FUSION_API FToolWindow : public FWindow
    {
        CE_CLASS(FToolWindow, FWindow)
    public:

        FToolWindow();

    protected:

        void Construct() override;

        FStyledWidget* borderWidget = nullptr;
        FVerticalStack* rootBox = nullptr;

    public:

        FUSION_PROPERTY_WRAPPER(Background, borderWidget);

    };
    
} // namespace CE

#include "FToolWindow.rtti.h"
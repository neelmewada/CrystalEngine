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
        FImage* maximizeIcon = nullptr;
        FVerticalStack* content = nullptr;

    public:

        FUSION_PROPERTY_WRAPPER(Background, borderWidget);

        template<typename... TArgs> requires TValidate_Children<TArgs...>::Value and (sizeof...(TArgs) > 0)
        Self& Content(const TArgs&... childWidget)
        {
            if (content)
            {
                content->operator()(childWidget...);
            }
            return *this;
        }

    };
    
} // namespace CE

#include "FToolWindow.rtti.h"
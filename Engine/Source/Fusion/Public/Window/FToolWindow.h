#pragma once

namespace CE
{
    CLASS()
    class FUSION_API FToolWindow : public FWindow
    {
        CE_CLASS(FToolWindow, FWindow)
    public:

        FToolWindow();

        FStackBox* GetContentBox() const { return content; }

    protected:

        void Construct() override;

        FStyledWidget* borderWidget = nullptr;
        FVerticalStack* rootBox = nullptr;
        FImage* maximizeIcon = nullptr;
        FStackBox* content = nullptr;
        FTitleBar* titleBar = nullptr;
        FLabel* titleBarLabel = nullptr;

    public:

        FUSION_PROPERTY_WRAPPER(Background, borderWidget);
        FUSION_PROPERTY_WRAPPER(BorderWidth, borderWidget);
        FUSION_PROPERTY_WRAPPER(BorderColor, borderWidget);
        FUSION_PROPERTY_WRAPPER2(Background, titleBar, TitleBarBackground);

        FUSION_PROPERTY_WRAPPER2(Padding, content, ContentPadding);
        FUSION_PROPERTY_WRAPPER2(FillRatio, content, ContentFillRatio);

        FUSION_PROPERTY_WRAPPER2(Text, titleBarLabel, Title);

        FStackBoxDirection ContentDirection()
        {
            return content->Direction();
        }

        Self& ContentDirection(FStackBoxDirection contentDirection)
        {
            content->Direction(contentDirection);
            return *this;
        }

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
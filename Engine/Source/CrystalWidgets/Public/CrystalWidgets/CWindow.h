#pragma once

namespace CE::Widgets
{
    CLASS()
    class CRYSTALWIDGETS_API CWindow : public CWidget
    {
        CE_CLASS(CWindow, CWidget)
    public:

        CWindow();
        virtual ~CWindow();

        inline void SetPlatformWindow(PlatformWindow* window)
        {
            this->nativeWindow = window;
        }

    private:

        void OnWindowSizeChanged(PlatformWindow* window, u32 newWidth, u32 newHeight);

        PlatformWindow* nativeWindow = nullptr;
        DelegateHandle windowResizeDelegate = 0;

        RPI::Renderer2D* renderer = nullptr;

        friend class CApplication;
        friend class CWidget;
    };
    
} // namespace CE::Widgets

#include "CWindow.rtti.h"
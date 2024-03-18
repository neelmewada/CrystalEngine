#pragma once

namespace CE::Widgets
{
    class CPainter;

    ENUM()
    enum class CWindowType
    {
        MajorDockWindow,
        MinorDockWindow,
        ToolWindow,
    };
    ENUM_CLASS(CWindowType);

    CLASS()
    class CRYSTALWIDGETS_API CWindow : public CWidget
    {
        CE_CLASS(CWindow, CWidget)
    public:

        CWindow();
        virtual ~CWindow();

        void SetPlatformWindow(PlatformWindow* window);

        inline RHI::DrawListTag GetDrawListTag() const { return drawListTag; }

        const Array<RHI::DrawPacket*>& FlushDrawPackets(u32 imageIndex);

        void SetWindowType(CWindowType windowType);

        inline void SetTitle(const String& title) { this->title = title; }

    protected:

        void OnPaint(CPaintEvent* paintEvent) override;

        void Tick();

        void OnBeforeDestroy() override;

        void Construct() override;

        void OnWindowSizeChanged(PlatformWindow* window, u32 newWidth, u32 newHeight);

        void OnSubobjectDetached(Object* object) override;
        void OnSubobjectAttached(Object* object) override;

        FIELD()
        String title = "";

        FIELD(ReadOnly)
        CWindowType windowType = CWindowType::MajorDockWindow;

        PlatformWindow* nativeWindow = nullptr;
        DelegateHandle windowResizeDelegate = 0;

        CPainter* painter = nullptr;
        RPI::Renderer2D* renderer = nullptr;
        RHI::DrawListTag drawListTag = 0;

        friend class CApplication;
        friend class CWidget;
    };
    
} // namespace CE::Widgets

#include "CWindow.rtti.h"
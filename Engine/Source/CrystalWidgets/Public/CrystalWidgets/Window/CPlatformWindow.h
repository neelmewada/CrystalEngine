#pragma once

namespace CE::Widgets
{
    class CWindow;
    class CWidget;
    class CPainter;

    class CRYSTALWIDGETS_API CPlatformWindow
    {
    protected:
        virtual ~CPlatformWindow();

    public:

        CPlatformWindow(CWindow* owner, u32 width, u32 height, const PlatformWindowInfo& info);
        CPlatformWindow(CWindow* owner, PlatformWindow* nativeWindow);

        void GetWindowSize(u32* preferredWidth, u32* preferredHeight);

        void Show();
        void Hide();

        bool IsShown();

        const Array<RHI::DrawPacket*>& FlushDrawPackets(u32 imageIndex);

    private:

        void Init();

        void OnWindowSizeChanged(PlatformWindow* window, u32 newWidth, u32 newHeight);

    crystalwidgets_internal:

        void Tick();

        CWindow* owner = nullptr;
        b8 isDeleted = false;

        PlatformWindow* platformWindow = nullptr;
        RHI::SwapChain* swapChain = nullptr;

        CPainter* painter = nullptr;
        RPI::Renderer2D* renderer = nullptr;
        RHI::DrawListTag drawListTag = 0;

        DelegateHandle windowResizeDelegate = 0;

    };
    
} // namespace CE::Widgets

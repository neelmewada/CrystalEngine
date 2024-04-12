#pragma once

namespace CE::Widgets
{
    class CWindow;
    class CWidget;
    class CPainter;

    class CRYSTALWIDGETS_API CPlatformWindow
    {
    crystalwidgets_protected_internal:
        virtual ~CPlatformWindow();

	public:

        CPlatformWindow(CWindow* owner, u32 width, u32 height, const PlatformWindowInfo& info, CPlatformWindow* parentWindow = nullptr);
        CPlatformWindow(CWindow* owner, PlatformWindow* nativeWindow, CPlatformWindow* parentWindow = nullptr);

        void GetWindowSize(u32* preferredWidth, u32* preferredHeight);

        void Show();
        void Hide();

        bool IsShown();
        bool IsHidden();

        bool IsFocused();
        bool IsMinimized();

        bool IsAlwaysOnTop();

        const Array<RHI::DrawPacket*>& FlushDrawPackets(u32 imageIndex);

        PlatformWindow* GetPlatformWindow() const { return platformWindow; }

    private:

        void Init();

        void OnWindowSizeChanged(PlatformWindow* window, u32 newWidth, u32 newHeight);

    crystalwidgets_internal:

        void Tick();

        CWindow* owner = nullptr;
        b8 isDeleted = false;
        CPlatformWindow* parentWindow = nullptr;
        Array<CPlatformWindow*> childrenWindows{};

        PlatformWindow* platformWindow = nullptr;
        RHI::SwapChain* swapChain = nullptr;

        CPainter* painter = nullptr;
        RPI::Renderer2D* renderer = nullptr;
        RHI::DrawListTag drawListTag = 0;

        DelegateHandle windowResizeDelegate = 0;

    };
    
} // namespace CE::Widgets

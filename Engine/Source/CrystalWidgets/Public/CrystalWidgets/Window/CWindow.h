#pragma once

namespace CE::Widgets
{
    class CPainter;
    class CMenu;
    class CMenuItem;

    CLASS()
    class CRYSTALWIDGETS_API CWindow : public CWidget
    {
        CE_CLASS(CWindow, CWidget)
    public:

        CWindow();
        virtual ~CWindow();

        void SetPlatformWindow(PlatformWindow* window);

        const String& GetTitle() const { return title; }

        void SetTitle(const String& title) { this->title = title; }

        bool IsLayoutCalculationRoot() override { return true; }

        Vec2 CalculateIntrinsicSize(f32 width, f32 height) override;

        bool IsDockSpace();

        PlatformWindow* GetRootNativeWindow();

        virtual void Show();

        virtual void Hide();

        bool IsSubWidgetAllowed(Class* subWidgetClass) override;

        // - Rendering -

        RHI::DrawListTag GetDrawListTag() const { return drawListTag; }

        const Array<RHI::DrawPacket*>& FlushDrawPackets(u32 imageIndex);

    crystalwidgets_protected_internal:

        virtual void OnPlatformWindowSet() {}

        void UpdateLayoutIfNeeded() override;

        void OnAfterUpdateLayout() override;

        virtual void ConstructWindow();

        void OnPaint(CPaintEvent* paintEvent) override;

        void OnPaintOverlay(CPaintEvent* paintEvent) override;

        void HandleEvent(CEvent* event) override;

        void Tick();

        void OnBeforeDestroy() override;

        void Construct() override;

        void OnWindowSizeChanged(PlatformWindow* window, u32 newWidth, u32 newHeight);

        void OnSubobjectAttached(Object* object) override;
        void OnSubobjectDetached(Object* object) override;

        Rect GetVerticalScrollBarRect();

        FIELD()
        String title = "";

        FIELD()
        Vec2 windowSize = Vec2(0, 0);

        FIELD()
        b8 canBeClosed = true;

        FIELD()
        b8 canBeMinimized = true;

        FIELD()
        b8 canBeMaximized = true;

        FIELD()
        Array<CMenuItem*> menuItems{};

        Array<Rect> controlRects{};
        StaticArray<bool, 3> hoveredControls{};
        int clickedControlIdx = -1;
        b8 isVerticalScrollPressed = false;
        b8 isVerticalScrollHovered = false;

        PlatformWindow* nativeWindow = nullptr;
        DelegateHandle windowResizeDelegate = 0;

        CPainter* painter = nullptr;
        RPI::Renderer2D* renderer = nullptr;
        RHI::DrawListTag drawListTag = 0;
        RHI::SwapChain* swapChain = nullptr;

        friend class CApplication;
        friend class CWidget;
        friend class CDockSpace;

        template<typename TWindow> requires TIsBaseClassOf<CWindow, TWindow>::Value
        friend TWindow* CreateWindow(const String& name, CWidget* parent, PlatformWindow* nativeWindow, Class* windowClass);
    };

    template<typename TWindow> requires TIsBaseClassOf<CWindow, TWindow>::Value
    TWindow* CreateWindow(const String& name, CWidget* parent = nullptr, PlatformWindow* nativeWindow = nullptr, Class* windowClass = GetStaticClass<TWindow>())
    {
        if (windowClass == nullptr)
            windowClass = GetStaticClass<TWindow>();
        Object* outer = parent;
        if (outer == nullptr)
            outer = CApplication::Get();

        TWindow* window = CreateObject<TWindow>(outer, name, OF_NoFlags, windowClass);
        window->SetTitle(name);
        window->SetPlatformWindow(nativeWindow);
        window->ConstructWindow();
        return window;
    }

} // namespace CE::Widgets

#include "CWindow.rtti.h"
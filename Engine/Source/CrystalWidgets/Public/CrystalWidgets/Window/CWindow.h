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

        CPlatformWindow* GetRootNativeWindow();

        CPlatformWindow* GetCurrentNativeWindow() const { return nativeWindow; }

        virtual void Show();

        virtual void Hide();

        bool IsSubWidgetAllowed(Class* subWidgetClass) override;

        Vec2 GetWindowSize() const;

        bool IsViewportWindow() const;

        bool IsAlwaysOnTop() const;

        void SetAlwaysOnTop(bool set);

        CWidget* HitTest(Vec2 windowSpaceMousePos) override;

        void AttachSubWindow(CWindow* subWindow);
        void DetachSubWindow(CWindow* subWindow);

    protected:

        virtual void OnClickClose();

        virtual void OnPlatformWindowSet();

        void UpdateLayoutIfNeeded() override;

        void OnAfterUpdateLayout() override;

        void OnPaint(CPaintEvent* paintEvent) override;

        void OnPaintOverlay(CPaintEvent* paintEvent) override;

        void HandleEvent(CEvent* event) override;

        void OnBeforeDestroy() override;

        void Construct() override;

        void OnSubobjectAttached(Object* object) override;
        void OnSubobjectDetached(Object* object) override;

        Rect GetVerticalScrollBarRect();

        virtual bool WindowHitTest(PlatformWindow* window, Vec2 position);

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
        b8 clearScreen = true;

        FIELD()
        Array<CMenuItem*> menuItems{};

        FIELD()
        Array<CWindow*> attachedWindows{};

        Array<Rect> controlRects{};
        StaticArray<bool, 3> hoveredControls{};
        int clickedControlIdx = -1;
        b8 isVerticalScrollPressed = false;
        b8 isVerticalScrollHovered = false;

        CPlatformWindow* nativeWindow = nullptr;

        friend class CApplication;
        friend class CWidget;
        friend class CDockSpace;

        CE_WIDGET_FRIENDS()
    };

    template<typename TWindow> requires TIsBaseClassOf<CWindow, TWindow>::Value
    TWindow* CreateWindow(Object* outer, const String& name, PlatformWindow* nativeWindow, Class* windowClass = GetStaticClass<TWindow>())
    {
        if (windowClass == nullptr)
            windowClass = GetStaticClass<TWindow>();

        if (outer == nullptr)
            outer = CApplication::Get();

        TWindow* window = CreateObject<TWindow>(outer, name, OF_NoFlags, windowClass);
        window->SetPlatformWindow(nativeWindow);
        return window;
    }

    template<typename TWindow> requires TIsBaseClassOf<CWindow, TWindow>::Value
    TWindow* CreateWindow(const String& name, PlatformWindow* nativeWindow, Class* windowClass = GetStaticClass<TWindow>())
    {
        return CreateWindow<TWindow>(CApplication::Get(), name, nativeWindow, windowClass);
    }

} // namespace CE::Widgets

#include "CWindow.rtti.h"
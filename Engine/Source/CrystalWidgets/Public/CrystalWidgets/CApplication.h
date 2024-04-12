#pragma once

namespace CE::Widgets
{
    class CWidget;
    class CWindow;
    class CStyleSheet;
    class CPlatformWindow;

    class CWidgetException : public std::exception
    {
    public:

        CWidgetException(const String& message) : message(message)
        {}

        virtual const char* what() const noexcept override
        {
            return message.GetCString();
        }

        const String& GetMessage() const { return message; }

    private:
        String message{};
    };

    class CWidgetResourceLoader
    {
    protected:
        virtual ~CWidgetResourceLoader() = default;

    public:

        virtual RPI::Texture* LoadImage(const Name& assetPath) = 0;

    };

    struct CApplicationStyleConstants
    {
        f32 scrollRectWidth = 10.0f;
        f32 minScrollRectSize = 20.0f;
        f32 scrollSizeBuffer = 1.0f;
    };

    struct CApplicationInitInfo
    {
        RPI::Shader* draw2dShader = nullptr;
        Name defaultFontName = "";
        RPI::FontAtlasAsset* defaultFont = nullptr;
        u32 numFramesInFlight = 2;
        RHI::FrameScheduler* scheduler = nullptr;
        CWidgetResourceLoader* resourceLoader = nullptr;
        CApplicationStyleConstants styleConstants{};
    };

    CLASS()
    class CRYSTALWIDGETS_API CApplication final : public Object, public ApplicationMessageHandler
    {
        CE_CLASS(CApplication, Object)
    public:

        CApplication();
        virtual ~CApplication();

        static CApplication* Get();
        static CApplication* TryGet();

        void Initialize(const CApplicationInitInfo& initInfo);
        void Shutdown();

        void Tick();
        
        CWindow* CreateWindow(const String& name, const String& title, PlatformWindow* nativeWindow);

        void RegisterFont(Name fontName, RPI::FontAtlasAsset* fontAtlas);

        void SetGlobalStyleSheet(CStyleSheet* globalStyleSheet);

        CStyleSheet* GetGlobalStyleSheet() const { return globalStyleSheet; }

        void LoadGlobalStyleSheet(const IO::Path& path);

        void BuildFrameGraph();

        void SetDrawListMasks(RHI::DrawListMask& outMask);

        void FlushDrawPackets(RHI::DrawListContext& drawList, u32 imageIndex);
        void SubmitDrawPackets(RHI::DrawListContext& drawList);

        RHI::FrameScheduler* GetFrameScheduler() const { return scheduler; }

        RPI::Texture* LoadImage(const Name& assetpath);

        Vec2 CalculateTextSize(const String& text, f32 fontSize, Name fontName, f32 width = 0.0f);
        Vec2 CalculateTextOffsets(Array<Rect>& outOffsetRects, const String& text, f32 fontSize, Name fontName, f32 width = 0.0f);

        CApplicationStyleConstants& GetStyleConstants() { return styleConstants; }

        Rect GetScreenBounds(int displayIndex);

    crystalwidgets_internal:

        void OnWidgetDestroyed(CWidget* widget);

        void OnWindowResized(PlatformWindow* window, u32 newWidth, u32 newHeight) override;

        void OnWindowRestored(PlatformWindow* window) override;

        void OnWindowDestroyed(PlatformWindow* window) override;

        void OnWindowClosed(PlatformWindow* window) override;

        void OnWindowCreated(PlatformWindow* window) override;

        RPI::Shader* draw2dShader = nullptr;
        Name defaultFontName = "";
        RPI::FontAtlasAsset* defaultFont = nullptr;
        u32 numFramesInFlight = 2;

        HashMap<Name, RPI::FontAtlasAsset*> registeredFonts{};

        void PushCursor(CCursor cursor);
        CCursor GetTopCursor();
        void PopCursor();

        void SetFocus(CWidget* widget);

    crystalwidgets_internal:

        void OnSubobjectDetached(Object* object) override;
        void OnSubobjectAttached(Object* object) override;

        FIELD()
        CStyleSheet* globalStyleSheet = nullptr;

        CApplicationStyleConstants styleConstants{};

        Array<CPlatformWindow*> platformWindows{};

        RHI::FrameScheduler* scheduler = nullptr;

        Array<CWidget*> destructionQueue{};

        Array<CCursor> cursorStack{};
        Array<CTimer*> timers{};

        KeyModifier keyModifierStates{};
        Array<bool> keyPressStates{};

        CWidgetResourceLoader* resourceLoader = nullptr;

        Array<CWidget*> hoveredWidgetsStack = {};
        StaticArray<CWidget*, 6> widgetsPressedPerMouseButton{};
        CWidget* draggedWidget = nullptr;
        CWidget* focusWidget = nullptr;
        CWidget* curFocusedWidget = nullptr;

        Vec2 prevMousePos = Vec2();

        HashMap<Name, RPI::Texture*> texturesByPath{};

        friend class CWidget;
        friend class CTimer;
    };

} // namespace CE::Widgets

#include "CApplication.rtti.h"
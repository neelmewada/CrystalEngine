#pragma once

namespace CE::Widgets
{
    class CWidget;
    class CWindow;
    class CStyleSheet;

    class CWidgetException : public std::exception
    {
    public:

        CWidgetException(const String& message) : message(message)
        {}

        virtual const char* what() const noexcept override
        {
            return message.GetCString();
        }

    private:
        String message{};
    };

    struct CApplicationInitInfo
    {
        RPI::Shader* draw2dShader = nullptr;
        Name defaultFontName = "";
        RPI::FontAtlasAsset* defaultFont = nullptr;
        u32 numFramesInFlight = 2;
    };

    CLASS()
    class CRYSTALWIDGETS_API CApplication final : public Object, public ApplicationMessageHandler
    {
        CE_CLASS(CApplication, Object)
    public:

        CApplication();
        virtual ~CApplication();

        static CApplication* Get();

        void Initialize(const CApplicationInitInfo& initInfo);
        void Shutdown();

        void Tick();
        
        CWindow* CreateWindow(const String& name, const String& title, PlatformWindow* nativeWindow);

        void RegisterFont(Name fontName, RPI::FontAtlasAsset* fontAtlas);

        void SetGlobalStyleSheet(CStyleSheet* globalStyleSheet);

        CStyleSheet* GetGlobalStyleSheet() const { return globalStyleSheet; }

        void LoadGlobalStyleSheet(const IO::Path& path);

    crystalwidgets_internal:

        void OnWindowResized(PlatformWindow* window, u32 newWidth, u32 newHeight) override;

        RPI::Shader* draw2dShader = nullptr;
        Name defaultFontName = "";
        RPI::FontAtlasAsset* defaultFont = nullptr;
        u32 numFramesInFlight = 2;

        HashMap<Name, RPI::FontAtlasAsset*> registeredFonts{};

    private:

        void OnSubobjectDetached(Object* object) override;
        void OnSubobjectAttached(Object* object) override;

        FIELD(ReadOnly)
        Array<CWindow*> windows{};

        FIELD()
        CStyleSheet* globalStyleSheet = nullptr;

    };

} // namespace CE::Widgets

#include "CApplication.rtti.h"
#pragma once

namespace CE::Widgets
{
    class CWidget;
    class CWindow;

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

    CLASS()
    class CRYSTALWIDGETS_API CApplication final : public Object, public ApplicationMessageHandler
    {
        CE_CLASS(CApplication, Object)
    public:

        CApplication();
        virtual ~CApplication();

        static CApplication* Get();

        void Initialize();
        void Shutdown();

        void Tick();

        CWindow* CreateWindow(const String& name, const String& title);

    private:


    };

} // namespace CE::Widgets

#include "CApplication.rtti.h"
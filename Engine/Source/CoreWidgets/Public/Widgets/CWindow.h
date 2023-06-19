#pragma once

namespace CE::Widgets
{

    CLASS()
    class COREWIDGETS_API CWindow : public CWidget
    {
        CE_CLASS(CWindow, CE::Widgets::CWidget)
    public:
        
        CWindow();
        virtual ~CWindow();
        
        // - Public API -
        
        bool IsShown() const
        {
            return isShown;
        }
        
        virtual bool IsWindow() override { return true; }
        
        void Show();
        void Hide();
        
        void SetTitle(const String& title);
        
        const String& GetTitle() const
        {
            return windowTitle;
        }
        
        void AddSubWidget(CWidget* subWidget);
        void RemoveSubWidget(CWidget* subWidget);
        
    protected:
        virtual void OnDrawGUI() override;

		void HandleEvent(CEvent* event) override;
        
        FIELD()
        String windowTitle{};
        
        FIELD()
        b8 isShown = true;

		FIELD()
		b8 isFullscreen = false;
    };
    
} // namespace CE::Widgets

#include "CWindow.rtti.h"

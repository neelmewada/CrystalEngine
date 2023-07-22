#pragma once

namespace CE::Widgets
{

    CLASS()
    class COREWIDGETS_API CWindow : public CWidget
    {
        CE_CLASS(CWindow, CWidget)
    public:
        
        CWindow();
        virtual ~CWindow();
        
        // - Public API -
        
        bool IsShown() const
        {
            return isShown;
        }
        
        virtual bool IsWindow() override { return true; }

		virtual Vec2 CalculateIntrinsicContentSize(f32 width, f32 height) override;

		inline bool AllowHorizontalScroll() const { return allowHorizontalScroll; }
		inline void SetAllowHorizontalScroll(bool value) { allowHorizontalScroll = value; }

		inline bool AllowVerticalScroll() const { return allowVerticalScroll; }
		inline void SetAllowVerticalScroll(bool value) { allowVerticalScroll = value; }

		inline bool IsFullscreen() const { return isFullscreen; }
		inline void SetFullscreen(bool value) { isFullscreen = value; }
        
        void Show();
        void Hide();
        
        void SetTitle(const String& title);
        
        const String& GetTitle() const
        {
            return windowTitle;
        }
        
        void AddSubWidget(CWidget* subWidget);
        void RemoveSubWidget(CWidget* subWidget);

		virtual void OnAfterComputeStyle() override;

		inline bool IsDockSpaceWindow() const { return isDockSpaceWindow; }
		void SetAsDockSpaceWindow(bool set);

	public:

		// Params: Old Size, New Size
		CE_SIGNAL(OnWindowResized, Vec2, Vec2);
        
    protected:
        
		virtual void OnDrawGUI() override;

		void HandleEvent(CEvent* event) override;

		Color FetchBackgroundColor(CStateFlag state, CSubControl subControl);
        
        FIELD()
        String windowTitle{};
        
        FIELD()
        b8 isShown = true;

		FIELD()
		b8 isFullscreen = false;

		FIELD()
		b8 allowHorizontalScroll = false;

		FIELD()
		b8 allowVerticalScroll = false;

		FIELD()
		Vec2 windowSize{};

		FIELD()
		b8 isDockSpaceWindow{};
		String dockSpaceId{};

		Color titleBar{};
		Color titleBarActive{};

		Color tab{};
		Color tabActive{};
		Color tabHovered{};
    };
    
} // namespace CE::Widgets

#include "CWindow.rtti.h"

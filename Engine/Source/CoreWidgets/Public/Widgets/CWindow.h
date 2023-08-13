#pragma once

namespace CE::Widgets
{
	class CMenuBar;
	class CPopup;

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

		inline Vec2i GetScreenSize() const { return screenSize; }
		inline void SetScreenSize(const Vec2i& size) { this->screenSize = size; }

		inline Vec2 GetWindowSize() const { return windowSize; }
		inline Vec2 GetWindowPos() const { return windowPos; }

		/// Could be: HWND, SDL_Window*, etc depending on platform
		inline void* GetPlatformHandle() const { return platformHandle; }

		inline CMenuBar* GetMenuBar() const { return menuBar; }
		inline void SetMenuBar(CMenuBar* menuBar) { this->menuBar = menuBar; }
        
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

		virtual void HandleEvent(CEvent* event) override;

		virtual void OnSubobjectAttached(Object* subobject) override;
		virtual void OnSubobjectDetached(Object* subobject) override;

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

		FIELD(ReadOnly)
		Vec2 windowSize{};

		FIELD(ReadOnly)
		Vec2 windowPos{};

		FIELD()
		b8 isDockSpaceWindow{};

		FIELD(ReadOnly)
		CMenuBar* menuBar = nullptr;

		FIELD(ReadOnly)
		Vec2i screenSize = Vec2i();

		FIELD(ReadOnly)
		Array<CPopup*> attachedPopups{};

		void* platformHandle = nullptr;

		String dockSpaceId{};

		Color titleBar{};
		Color titleBarActive{};

		Color tab{};
		Color tabActive{};
		Color tabHovered{};
    };
    
} // namespace CE::Widgets

#include "CWindow.rtti.h"

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
        
        void Show();
        void Hide();
        
        void SetTitle(const String& title);
        
        const String& GetTitle() const
        {
            return windowTitle;
        }
        
        void AddSubWidget(CWidget* subWidget);
        void RemoveSubWidget(CWidget* subWidget);

		virtual void OnBeforeComputeStyle() override;

		inline bool IsDockSpaceWindow() const { return isDockSpaceWindow; }
		inline void SetAsDockSpaceWindow(bool set) { isDockSpaceWindow = set; }

	public:

		// Params: Old Size, New Size
		CE_SIGNAL(OnWindowResized, Vec2, Vec2);
        
    protected:
        virtual void OnDrawGUI() override;

		void HandleEvent(CEvent* event) override;
        
        FIELD()
        String windowTitle{};
        
        FIELD()
        b8 isShown = true;

		FIELD()
		b8 isFullscreen = false;

		FIELD()
		b8 allowHorizontalScroll = false;

		FIELD()
		Vec2 windowSize{};

		FIELD()
		b8 isDockSpaceWindow{};
		String dockSpaceId{};
    };
    
} // namespace CE::Widgets

#include "CWindow.rtti.h"

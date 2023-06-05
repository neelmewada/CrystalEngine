#pragma once

namespace CE::Widgets
{
	namespace Internal
	{
		struct WidgetInitializer;
	}

	class CWindow;

	CLASS(Abstract)
	class COREWIDGETS_API CWidget : public Object
	{
		CE_CLASS(CWidget, CE::Object)
	public:

		virtual ~CWidget();

		// - Public API -
        
        void BuildWidget();

        // For internal use only!
		virtual void RenderGUI();
        
        virtual bool IsWindow() { return false; }
		virtual bool IsContainer() { return IsWindow(); }

		bool IsFocused() const { return isFocused; }
        
        void SetWidgetFlags(WidgetFlags flags);
        
        WidgetFlags GetWidgetFlags() const;
        
        CStyle& GetStyle()
        {
            return style;
        }
        
        void SetStyle(const CStyle& style)
        {
            this->style = style;
        }

		CWidget* GetOwner();

		CWindow* GetOwnerWindow();

	protected:

		CWidget();

		// Events

		virtual void HandleEvent(CEvent* event);

		virtual void OnMouseClicked(int mouseButton) {}
		virtual void OnMouseDoubleClicked(int mouseButton) {}

		// Protected API

		virtual void OnWidgetAttached(CWidget* widget) {}
		virtual void OnWidgetDetached(CWidget* widget) {}

		virtual void OnAttachedTo(CWidget* parent) {}
		virtual void OnDetachedFrom(CWidget* parent) {}

		/// Abstract method. Must be overriden to call low level GUI draw functions.
		virtual void OnDrawGUI() = 0;
        
        /// Override this method to build child widget hierarchy
        virtual void Build() {}
        
        virtual void BeginStyle();
        virtual void EndStyle();

		/// Must be called at the end of all GUI draw calls
		virtual void PollEvents();

	private:

		virtual void OnSubobjectAttached(Object* subobject) override;
		virtual void OnSubobjectDetached(Object* subobject) override;

	protected: // Fields

		FIELD()
		WidgetFlags widgetFlags{};

		FIELD()
		Array<CWidget*> attachedWidgets{};
        
        FIELD()
        CStyle style{};
        
    private:

        b8 isBuilt = false;
		b8 isHovered = false;
		Vec2 prevHoverPos{};

		b8 isFocused = false;
		b8 isLeftMouseDown = false;
        
	private:
        u32 pushedColors = 0;
        u32 pushedVars = 0;
        
	};
    
} // namespace CE::Widgets

#include "CWidget.rtti.h"



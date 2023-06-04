#pragma once

namespace CE::Widgets
{
	namespace Internal
	{
		struct WidgetInitializer;
	}

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

		bool IsFocused() const { return isFocused; }
		bool IsLeftClicked() const { return isLeftClicked; }
		bool IsRightClicked() const { return isRightClicked; }
        
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

	protected:

		CWidget();

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
		b8 isFocused = false;
		b8 isLeftClicked = false, isRightClicked = false, isMiddleClicked = false;
        
        u32 pushedColors = 0;
        u32 pushedVars = 0;
        
	};
    
} // namespace CE::Widgets

#include "CWidget.rtti.h"



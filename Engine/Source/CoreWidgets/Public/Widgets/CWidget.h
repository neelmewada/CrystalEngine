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

        // For internal use only!
		virtual void RenderGUI();

		bool IsFocused() const { return isFocused; }
		bool IsLeftClicked() const { return isLeftClicked; }
		bool IsRightClicked() const { return isRightClicked; }
        
        void SetWidgetFlags(WidgetFlags flags);
        
        WidgetFlags GetWidgetFlags() const;

	protected:

		CWidget();

		// Protected API

		virtual void OnWidgetAttached(CWidget* widget) {}
		virtual void OnWidgetDetached(CWidget* widget) {}

		virtual void OnAttachedTo(CWidget* parent) {}
		virtual void OnDetachedFrom(CWidget* parent) {}

		/// Abstract method. Override it to call low level GUI functions.
		virtual void OnDrawGUI() = 0;

	private:

		virtual void OnSubobjectAttached(Object* subobject) override;
		virtual void OnSubobjectDetached(Object* subobject) override;

	protected: // Fields

		FIELD()
		WidgetFlags widgetFlags{};

		FIELD()
		Array<CWidget*> attachedWidgets{};

		b8 isHovered = false;
		b8 isFocused = false;
		b8 isLeftClicked = false, isRightClicked = false, isMiddleClicked = false;
	};
    
} // namespace CE::Widgets

#include "CWidget.rtti.h"



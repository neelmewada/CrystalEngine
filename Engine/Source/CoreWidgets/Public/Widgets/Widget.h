#pragma once

namespace CE::Widgets
{
	namespace Internal
	{
		struct WidgetInitializer;
	}

	CLASS(Abstract)
	class COREWIDGETS_API Widget : public Object
	{
		CE_CLASS(Widget, CE::Object)
	public:

		virtual ~Widget();

		// - Public API -

		virtual void RenderGUI();

		bool IsFocused() const { return isFocused; }
		bool IsLeftClicked() const { return isLeftClicked; }
		bool IsRightClicked() const { return isRightClicked; }

	protected:

		Widget();

		// Protected API

		virtual void OnWidgetAttached(Widget* widget) {}
		virtual void OnWidgetDetached(Widget* widget) {}

		virtual void OnAttachedTo(Widget* parent) {}
		virtual void OnDetachedFrom(Widget* parent) {}

		/// Abstract method. Override it to call low level GUI functions.
		virtual void OnDrawGUI() = 0;

	private:

		void ConstructInternal();
		void ConstructInternal(Internal::WidgetInitializer* initializer);

		virtual void OnSubobjectAttached(Object* subobject) override;
		virtual void OnSubobjectDetached(Object* subobject) override;

	protected: // Fields

		FIELD()
		WidgetFlags widgetFlags{};

		FIELD()
		Array<Widget*> attachedWidgets{};

		b8 isHovered = false;
		b8 isFocused = false;
		b8 isLeftClicked = false, isRightClicked = false, isMiddleClicked = false;
	};
    
} // namespace CE::Widgets

#include "Widget.rtti.h"



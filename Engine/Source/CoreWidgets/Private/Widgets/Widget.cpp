
#include "CoreWidgets.h"

namespace CE::Widgets
{

	Widget::Widget()
	{
		ConstructInternal();
	}

	void Widget::ConstructInternal()
	{
		auto& ctx = Internal::WidgetThreadContext::Get();
		auto initializer = ctx.TopInitializer();
		ASSERT(initializer != nullptr, "A Widget is created without setting a WidgetInitializer. This happens when a widget is created without using the CreateWidget function");
		ctx.PopInitializer();
		ConstructInternal(initializer);
	}

	void Widget::ConstructInternal(Internal::WidgetInitializer* initializer)
	{
		widgetFlags = initializer->flags;
	}

	void Widget::OnSubobjectAttached(Object* subobject)
	{
		if (subobject == nullptr)
			return;
		if (subobject->GetClass()->IsSubclassOf<Widget>())
		{
			Widget* subWidget = (Widget*)subobject;
			attachedWidgets.Add(subWidget);
			OnWidgetAttached(subWidget);
			subWidget->OnAttachedTo(this);
		}
	}

	void Widget::OnSubobjectDetached(Object* subobject)
	{
		if (subobject == nullptr)
			return;
		if (subobject->GetClass()->IsSubclassOf<Widget>())
		{
			Widget* subWidget = (Widget*)subobject;
			attachedWidgets.Remove(subWidget);
			OnWidgetDetached(subWidget);
			subWidget->OnDetachedFrom(this);
		}
	}
    
	Widget::~Widget()
	{

	}

	void Widget::RenderGUI()
	{
		OnDrawGUI();

		isFocused = GUI::IsItemFocused();
		isHovered = GUI::IsItemHovered();
		isLeftClicked = GUI::IsItemClicked(GUI::MouseButton::Left);
		isRightClicked = GUI::IsItemClicked(GUI::MouseButton::Right);
		isMiddleClicked = GUI::IsItemClicked(GUI::MouseButton::Middle);
	}
	

} // namespace CE::Widgets


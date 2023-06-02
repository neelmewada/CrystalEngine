#pragma once

#include "CoreMinimal.h"
#include "CoreGUI.h"

// Widgets
#include "Widgets/Definitions.h"
#include "Style/CStyle.h"
#include "Widgets/CWidget.h"
#include "Widgets/WidgetManager.h"
#include "Widgets/CWindow.h"

#include "Widgets/CLabel.h"

namespace CE::Widgets
{
    
	COREWIDGETS_API Package* GetWidgetsTransientPackage();

	namespace Internal
	{
		struct WidgetInitializer
		{
			WidgetFlags flags = WidgetFlags::None;
		};

		class WidgetThreadContext : public ThreadSingleton<WidgetThreadContext>
		{
		public:
			WidgetThreadContext() {}
			virtual ~WidgetThreadContext() {}

			void PushInitializer(WidgetInitializer* initializer)
			{
				widgetInitializerStack.Add(initializer);
			}

			void PopInitializer()
			{
				if (!widgetInitializerStack.IsEmpty())
					widgetInitializerStack.RemoveAt(widgetInitializerStack.GetSize() - 1);
			}

			WidgetInitializer* TopInitializer()
			{
				return widgetInitializerStack.IsEmpty() ? nullptr : widgetInitializerStack.GetLast();
			}

		private:
			friend class ThreadSingleton<ObjectThreadContext>;

			Array<WidgetInitializer*> widgetInitializerStack{};
		};
	}

	template<typename TWidget> requires TIsBaseClassOf<CWidget, TWidget>::Value
	TWidget* CreateWidget(Object* owner = nullptr,
		String widgetName = "Widget",
		WidgetFlags widgetFlags = WidgetFlags::None,
		ClassType* widgetClass = TWidget::Type(),
		ObjectFlags objectFlags = OF_NoFlags)
	{
		Object* ownerObject = owner;
		if (ownerObject == nullptr)
			ownerObject = GetWidgetsTransientPackage();
		if (!widgetClass->CanBeInstantiated()) // widgetClass should NOT be the `Widget` class itself & it shouldn't be abstract
			return nullptr;
		if (!widgetClass->IsSubclassOf(CWidget::Type()))
			return nullptr;

		Internal::WidgetInitializer init{};
		init.flags = widgetFlags;

		Internal::WidgetThreadContext::Get().PushInitializer(&init);

		CWidget* widget = CreateObject<TWidget>(ownerObject, widgetName, objectFlags, widgetClass);

		if (widget == nullptr) // Failed to create widget
		{
			Internal::WidgetThreadContext::Get().PopInitializer(); // Only pop when failed, because the Widget class auto-pops initializer on success
		}
		
		return (TWidget*)widget;
	}

}


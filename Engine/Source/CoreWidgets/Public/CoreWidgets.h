#pragma once

#include "CoreMinimal.h"
#include "CoreGUI.h"

#include "Widgets/Definitions.h"

#include "Data/CVariant.h"
#include "Data/CDataModel.h"

#include "Style/CStyle.h"
#include "Style/CStyleManager.h"
#include "Event/CEvent.h"

#include "Widgets/CWidget.h"
#include "Widgets/WidgetManager.h"
#include "Widgets/CWindow.h"

// Widgets
#include "Widgets/CLabel.h"
#include "Widgets/CButton.h"
#include "Widgets/CStackLayout.h"
#include "Widgets/CTableView.h"

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
		ClassType* widgetClass = TWidget::Type(),
		ObjectFlags objectFlags = OF_NoFlags)
	{
		TWidget* widget = CreateObject<TWidget>(owner, widgetName, objectFlags, widgetClass);
		widget->ConstructInternal();
		return widget;
	}

}


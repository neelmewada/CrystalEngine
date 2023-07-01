#pragma once

#include "CoreMinimal.h"
#include "CoreRHI.h"
#include "CoreGUI.h"

#include "yoga/Yoga.h"
#include "yoga/YGNode.h"
#include "yoga/YGStyle.h"

#include "Widgets/Definitions.h"

#include "Data/CVariant.h"
#include "Data/CDataModel.h"

#include "Style/CStyle.h"

#include "Style/Parser/CSSParserToken.h"
#include "Style/Parser/CSSTokenizer.h"
#include "Style/Parser/CSSParser.h"

#include "Style/CStyleManager.h"
#include "Event/CEvent.h"

#include "Widgets/CWidget.h"
#include "Widgets/CWindow.h"
//#include "Widgets/CDockContainer.h"

// Widgets
#include "Widgets/CLabel.h"
#include "Widgets/CButton.h"
#include "Widgets/CStackLayout.h"
#include "Widgets/CTableView.h"
//#include "Widgets/CTabView.h"
//#include "Widgets/CCollapsibleSection.h"
//#include "Widgets/CGridLayout.h"
//#include "Widgets/CSelectableWidget.h"
//#include "Widgets/CSelectableGroup.h"

// Input Widgets
//#include "Widgets/Input/CCheckbox.h"
#include "Widgets/Input/CTextInput.h"
//#include "Widgets/Input/CFloatInput.h"
//#include "Widgets/Input/CIntInput.h"

namespace CE::Widgets
{
    
	COREWIDGETS_API Package* GetWidgetsTransientPackage();


	template<typename TWidget> requires TIsBaseClassOf<CWidget, TWidget>::Value
	TWidget* CreateWidget(Object* owner = nullptr,
		String widgetName = "Widget",
		ClassType* widgetClass = TWidget::Type(),
		ObjectFlags objectFlags = OF_NoFlags)
	{
		TWidget* widget = CreateObject<TWidget>(owner, widgetName, objectFlags, widgetClass);
		widget->Construct();
		return widget;
	}

}


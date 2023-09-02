#pragma once

#include "Core.h"
#include "CoreMedia.h"
#include "CoreRHI.h"
#include "CoreGUI.h"

// Yoga
#include "yoga/Yoga.h"
#include "yoga/YGNode.h"
#include "yoga/YGStyle.h"

#include "Widgets/Definitions.h"

// Data/Models
#include "Data/CVariant.h"
#include "Data/CDataModel.h"

// Styling
#include "Style/CStyle.h"
#include "Style/CFontManager.h"
#include "Style/CStyleSheet.h"

// CSS Parser
#include "Style/CSS/CSSParserToken.h"
#include "Style/CSS/CSSTokenizer.h"
#include "Style/CSS/CSSSelector.h"
#include "Style/CSS/CSSStyleSheet.h"
#include "Style/CSS/CSSParser.h"

// Misc
#include "Style/CStyleManager.h"
#include "Event/CEvent.h"

// Base CWidget class
#include "Widgets/CWidget.h"

// Widgets
#include "Widgets/CLabel.h"
#include "Widgets/CButton.h"
#include "Widgets/CSpacer.h"
#include "Widgets/CTableWidget.h"
#include "Widgets/CTableView.h"
#include "Widgets/CTreeView.h"
#include "Widgets/CSeparator.h"
#include "Widgets/CTabWidget.h"
#include "Widgets/CSelectableWidget.h"
#include "Widgets/CSelectableGroup.h"
#include "Widgets/CCollapsibleSection.h"
#include "Widgets/CScrollView.h"
#include "Widgets/CContainerWidget.h"
#include "Widgets/CLayoutGroup.h"
#include "Widgets/CSplitView.h"

// Popups
#include "Widgets/Popups/CPopup.h"
#include "Widgets/Popups/CNotification.h"

// Input
#include "Widgets/Input/CTextInput.h"

// Menu
#include "Widgets/Menu/CMenu.h"
#include "Widgets/Menu/CMenuItem.h"
#include "Widgets/Menu/CContextMenu.h"
#include "Widgets/Menu/CMenuBar.h"

// ToolBar
#include "Widgets/Tool/CToolBarItem.h"
#include "Widgets/Tool/CToolBar.h"

// CWindow class
#include "Widgets/CWindow.h"

// Debugging
#include "Debug/CWidgetDebugger.h"


namespace CE::Widgets
{
    
	COREWIDGETS_API Package* GetWidgetsTransientPackage();

	COREWIDGETS_API CWidgetDebugger* GetWidgetDebugger();

	template<typename TWidget> requires TIsBaseClassOf<CWidget, TWidget>::Value
	TWidget* CreateWidget(Object* owner = (Object*)GetWidgetsTransientPackage(),
		String widgetName = "Widget",
		ClassType* widgetClass = TWidget::Type(),
		ObjectFlags objectFlags = OF_NoFlags)
	{
		TWidget* widget = CreateObject<TWidget>(owner, widgetName, objectFlags, widgetClass);
		((CWidget*)widget)->Construct();
		return widget;
	}

}


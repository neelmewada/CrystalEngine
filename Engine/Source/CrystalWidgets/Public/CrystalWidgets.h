#pragma once

#include "Core.h"
#include "CoreApplication.h"
#include "CoreRPI.h"

// Yoga
#include "yoga/Yoga.h"

#define CE_WIDGET_FRIENDS()\
    friend class CWidget;\
    friend class CWindow;\
    friend class CToolWindow;\
	friend class CApplication;\
	friend class CTextInput;\
	friend class CLabel;\
	friend class CDockWindow;\
	friend class CDockSpace;\
	friend class CDockSplitView;\
	friend class CPlatformWindow;\
	friend class CStyleSheet;\
	friend class CSSStyleSheet;\
	friend class CTreeWidgetItem;\
	friend class CScrollBehavior;\
	friend class CMenuItem;\
	friend class CSSSelector;\
	friend class CSplitViewContainer;\
	friend class CSplitView;\
	friend class CComboBox;\


// Style
#include "CrystalWidgets/Style/FlexEnums.h"
#include "CrystalWidgets/Style/CStyle.h"

// App & Utils
#include "CrystalWidgets/Event/CEventType.h"
#include "CrystalWidgets/Event/CTimer.h"
#include "CrystalWidgets/CApplication.h"
#include "CrystalWidgets/Window/CPlatformWindow.h"

#include "CrystalWidgets/Style/CStyleSheet.h"

// CSS
#include "CrystalWidgets/Style/CSS/CSSParserToken.h"
#include "CrystalWidgets/Style/CSS/CSSTokenizer.h"
#include "CrystalWidgets/Style/CSS/CSSSelector.h"
#include "CrystalWidgets/Style/CSS/CSSStyleSheet.h"
#include "CrystalWidgets/Style/CSS/CSSParser.h"

// Painting
#include "CrystalWidgets/Painter/CPen.h"
#include "CrystalWidgets/Painter/CBrush.h"
#include "CrystalWidgets/Painter/CFont.h"
#include "CrystalWidgets/Painter/CPainter.h"

#include "CrystalWidgets/Event/CEvent.h"

#include "CrystalWidgets/Validation/CTextInputValidation.h"

// Data
#include "CrystalWidgets/Data/CModelIndex.h"
#include "CrystalWidgets/Data/CBaseItemModel.h"
#include "CrystalWidgets/Data/CFileSystemModel.h"

// Behaviors
#include "CrystalWidgets/Behavior/CBehavior.h"
#include "CrystalWidgets/Behavior/CScrollBehavior.h"

// Base Classes
#include "CrystalWidgets/Widget/CWidget.h"
#include "CrystalWidgets/Window/CWindow.h"
#include "CrystalWidgets/Window/CGameWindow.h"
#include "CrystalWidgets/Widget/CViewport.h"

#include "CrystalWidgets/Item/CItemSelectionModel.h"
#include "CrystalWidgets/Item/CBaseItemDelegate.h"
#include "CrystalWidgets/Item/CStandardItemDelegate.h"

// Popups
#include "CrystalWidgets/Popup/CPopup.h"

// Widgets
#include "CrystalWidgets/Widget/CLabel.h"
#include "CrystalWidgets/Widget/Input/CLabelButton.h"
#include "CrystalWidgets/Widget/Input/CButton.h"
#include "CrystalWidgets/Widget/CImage.h"
#include "CrystalWidgets/Widget/Input/CImageButton.h"
#include "CrystalWidgets/Widget/Input/CColorPicker.h"
#include "CrystalWidgets/Widget/Input/CCheckBox.h"
#include "CrystalWidgets/Widget/Input/CTextInput.h"
#include "CrystalWidgets/Widget/Input/CComboBox.h"
#include "CrystalWidgets/Widget/CTabWidget.h"
#include "CrystalWidgets/Widget/CTabWidgetContainer.h"
#include "CrystalWidgets/Widget/CScrollArea.h"
#include "CrystalWidgets/Widget/CSplitViewContainer.h"
#include "CrystalWidgets/Widget/CSplitView.h"
#include "CrystalWidgets/Widget/CCollapsibleSection.h"

#include "CrystalWidgets/Item/CBaseItemView.h"
#include "CrystalWidgets/Item/CTreeView.h"
#include "CrystalWidgets/Widget/CListWidgetItem.h"
#include "CrystalWidgets/Widget/CListWidget.h"
#include "CrystalWidgets/Widget/CTreeWidgetItem.h"
#include "CrystalWidgets/Widget/CTreeWidgetRow.h"
#include "CrystalWidgets/Widget/CTreeWidget.h"

//Menu
#include "CrystalWidgets/Menu/CMenu.h"
#include "CrystalWidgets/Menu/CMenuItem.h"

// Windows
#include "CrystalWidgets/Window/CDockSplitView.h"
#include "CrystalWidgets/Window/CDockSpace.h"
#include "CrystalWidgets/Window/CDockWindow.h"
#include "CrystalWidgets/Window/CToolWindow.h"

namespace CE::Widgets
{
    
}

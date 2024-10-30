#pragma once


namespace CE::Editor
{
	class ComponentTreeViewStyle;

	CLASS()
	class EDITORCORE_API EditorStyle : public FStyleSet
	{
		CE_CLASS(EditorStyle, FStyleSet)
	public:

		EditorStyle();

		static void Initialize();
		static void Shutdown();

	private:

		void InitializeDefault();

		void InitProjectBrowserStyle();

		void InitEditorStyle();

	public:

		FIELD()
		Color highlightColor = Color::RGBA(0, 112, 224);

		FIELD()
		FCustomButtonStyle* button = nullptr;

		FIELD()
		FCustomButtonStyle* highlightedButton = nullptr;

		FIELD()
		FCustomButtonStyle* windowCloseButton = nullptr;

		FIELD()
		FCustomButtonStyle* windowControlButton = nullptr;

		FIELD()
		FTextInputPlainStyle* textInput = nullptr;

		FIELD()
		FExpandableSectionStyle* expandableSection = nullptr;

		FIELD()
		FComboBoxPlainStyle* comboBox = nullptr;

		FIELD()
		FComboBoxItemPlainStyle* comboBoxItem = nullptr;

		FIELD()
		FTabViewStyle* tabView = nullptr;

		FIELD()
		FMenuPopupStyle* menuPopup = nullptr;

		FIELD()
		FMenuBarStyle* menuBar = nullptr;

		FIELD()
		FScrollBoxStyle* scrollBox = nullptr;

		FIELD()
		FToolWindowStyle* toolWindow = nullptr;

		FIELD()
		FImageButtonStyle* expandCaretButton = nullptr;

		// - Project Browser Window -

		FIELD()
		FToolWindowStyle* projectBrowserWindow = nullptr;

		FIELD()
		FTabViewStyle* projectBrowserTabView = nullptr;

		FIELD()
		FListViewStyle* projectBrowserListView = nullptr;

		// - Editor -

		FIELD()
		ComponentTreeViewStyle* componentTreeView = nullptr;

		FIELD()
		EditorDockspaceStyle* editorDockspace = nullptr;

		FIELD()
		EditorMinorDockspaceStyle* editorMinorDockspace = nullptr;

		FIELD()
		EditorDockTabStyle* editorDockTab = nullptr;

		FIELD()
		EditorDockTabStyle* editorMinorDockTab = nullptr;

		FIELD()
		FStyledWidgetStyle* editorToolBar = nullptr;

		FIELD()
		EditorMenuBarStyle* editorMenuBar = nullptr;

		FIELD()
		FTreeViewStyle* treeView = nullptr;

	};

} // namespace CE::Editor

#include "EditorStyle.rtti.h"
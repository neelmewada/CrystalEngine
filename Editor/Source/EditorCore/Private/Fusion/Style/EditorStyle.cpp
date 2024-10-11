#include "EditorCore.h"

namespace CE::Editor
{
	static EditorStyle* gEditorStyle = nullptr;

    EditorStyle::EditorStyle()
    {
	    
    }

    void EditorStyle::Initialize()
    {
		auto app = FusionApplication::TryGet();

		if (app)
		{
			FStyleManager* styleManager = app->GetStyleManager();

			FRootContext* rootContext = app->GetRootContext();

			gEditorStyle = CreateObject<EditorStyle>(nullptr, "RootEditorStyle");
			gEditorStyle->InitializeDefault();

			styleManager->RegisterStyleSet(gEditorStyle);
			rootContext->SetDefaultStyleSet(gEditorStyle);
		}
    }

    void EditorStyle::Shutdown()
    {
		if (gEditorStyle)
		{
			gEditorStyle->Destroy();
			gEditorStyle = nullptr;
		}
    }

    void EditorStyle::InitializeDefault()
    {
        if (!button)
        {
            button = CreateObject<FCustomButtonStyle>(this, "Button");
            Add(button);
        }

		button->background = Color::RGBA(56, 56, 56);
		button->hoveredBackground = Color::RGBA(95, 95, 95);
		button->pressedBackground = Color::RGBA(50, 50, 50);
		button->disabledBackground = Color::RGBA(40, 40, 40);
		button->cornerRadius = Vec4(1, 1, 1, 1) * 5;
		button->borderColor = Color::RGBA(24, 24, 24);
		button->borderWidth = 1.0f;

		GetDefaultWidget<FButton>()
			.Padding(Vec4(10, 5, 10, 5))
			.Style(this, button->GetName())
			;
		GetDefaultWidget<FTextButton>()
			.Padding(Vec4(10, 5, 10, 5))
			.Style(this, button->GetName())
			;

		if (!highlightedButton)
		{
			highlightedButton = CreateObject<FCustomButtonStyle>(this, "Button_Highlighted");
			Add("Button.Highlighted", highlightedButton);
		}

		highlightedButton->background = highlightColor;
		highlightedButton->hoveredBackground = Color::RGBA(14, 134, 255);
		highlightedButton->pressedBackground = Color::RGBA(0, 100, 200);
		highlightedButton->cornerRadius = Vec4(1, 1, 1, 1) * 5;
		highlightedButton->borderColor = Color::RGBA(24, 24, 24);
    	highlightedButton->borderWidth = 1.0f;

		if (!scrollBox)
		{
			scrollBox = CreateObject<FScrollBoxStyle>(this, "ScrollBox");
			Add(scrollBox);
		}

		GetDefaultWidget<FScrollBox>()
			.Style(this, scrollBox->GetName())
			;

		if (!windowCloseButton)
		{
			windowCloseButton = CreateObject<FCustomButtonStyle>(this, "Button_WindowClose");
			Add("Button.WindowClose", windowCloseButton);
		}

		windowCloseButton->background = Color::Clear();
		windowCloseButton->hoveredBackground = Color::RGBA(161, 57, 57);
		windowCloseButton->hoveredBackground = Color::RGBA(196, 43, 28);
		windowCloseButton->pressedBackground = Color::RGBA(178, 43, 30);
		windowCloseButton->borderColor = Color::Clear();
		windowCloseButton->borderWidth = 0.0f;
		windowCloseButton->cornerRadius = Vec4();
		windowCloseButton->contentMoveY = 0;

		if (!windowControlButton)
		{
			windowControlButton = CreateObject<FCustomButtonStyle>(this, "Button_WindowControl");
			Add("Button.WindowControl", windowControlButton);
		}

		windowControlButton->background = Color::Clear();
		windowControlButton->hoveredBackground = Color::RGBA(58, 58, 58);
		windowControlButton->pressedBackground = Color::RGBA(48, 48, 48);
		windowControlButton->borderColor = Color::Clear();
		windowControlButton->borderWidth = 0.0f;
		windowControlButton->cornerRadius = Vec4();
		windowControlButton->contentMoveY = 0;

		if (!textInput)
		{
			textInput = CreateObject<FTextInputPlainStyle>(this, "TextInput");
			Add(textInput);
		}

		textInput->background = textInput->hoverBackground = textInput->editingBackground = Color::RGBA(15, 15, 15);
		textInput->borderColor = Color::RGBA(60, 60, 60);
		textInput->editingBorderColor = highlightColor;
		textInput->hoverBorderColor = Color::RGBA(74, 74, 74);
		textInput->borderWidth = 1.0f;
		textInput->cornerRadius = Vec4(5, 5, 5, 5);

		GetDefaultWidget<FTextInput>()
			.Style(this, textInput->GetName());

		if (!comboBox)
		{
			comboBox = CreateObject<FComboBoxPlainStyle>(this, "ComboBox");
			Add(comboBox);
		}

		comboBox->background = Color::RGBA(15, 15, 15);
		comboBox->borderColor = Color::RGBA(60, 60, 60);
		comboBox->pressedBorderColor = comboBox->hoverBorderColor = Color::RGBA(74, 74, 74);
		comboBox->borderWidth = 1.0f;
		comboBox->cornerRadius = Vec4(5, 5, 5, 5);

		if (!comboBoxItem)
		{
			comboBoxItem = CreateObject<FComboBoxItemPlainStyle>(this, "ComboBoxItem");
			Add(comboBoxItem);
		}

		comboBoxItem->background = Color::Clear();
		comboBoxItem->hoverBackground = highlightColor;
		comboBoxItem->selectedBackground = Color::Clear();
		comboBoxItem->selectedShape = FShapeType::RoundedRect;
		comboBoxItem->shapeCornerRadius = Vec4(1, 1, 1, 1) * 3;
		comboBoxItem->selectedBorderColor = comboBoxItem->hoverBackground;
		comboBoxItem->borderWidth = 1.0f;

		GetDefaultWidget<FComboBox>()
			.ItemStyle(comboBoxItem)
			.Style(this, comboBox->GetName());

		if (!tabView)
		{
			tabView = CreateObject<FTabViewStyle>(this, "TabView");
			Add(tabView);
		}

		tabView->tabItemActiveBackground = Color::RGBA(50, 50, 50);
		tabView->tabItemHoverBackground = Color::RGBA(40, 40, 40);
		tabView->tabItemBackground = Color::RGBA(36, 36, 36);
		tabView->tabItemShape = FRoundedRectangle(3, 3, 0, 0);

		GetDefaultWidget<FTabView>()
			.Style(this, tabView->GetName());

		if (!menuPopup)
		{
			menuPopup = CreateObject<FMenuPopupStyle>(this, "MenuPopup");
			Add(menuPopup);
		}

		menuPopup->background = Color::RGBA(56, 56, 56);
		menuPopup->borderColor = Color::RGBA(26, 26, 26);
		menuPopup->borderWidth = 0.5f;
		menuPopup->itemPadding = Vec4(10, 5, 10, 5);
		menuPopup->itemHoverBackground = highlightColor;

		GetDefaultWidget<FMenuPopup>()
			.Style(this, menuPopup->GetName());

		GetDefaultWidget<EditorMenuPopup>()
			.Style(this, menuPopup->GetName());

		if (!menuBar)
		{
			menuBar = CreateObject<FMenuBarStyle>(this, "MenuBar");
			Add(menuBar);
		}

		menuBar->background = Color::Black();
		menuBar->itemPadding = Vec4(5, 5, 5, 5);
		menuBar->itemHoverBackground = highlightColor;

		GetDefaultWidget<FMenuBar>()
			.Style(this, menuBar->GetName());

		if (!toolWindow)
		{
			toolWindow = CreateObject<FToolWindowStyle>(this, "ToolWindow");
			Add(toolWindow);
		}

		GetDefaultWidget<FToolWindow>()
			.Style(this, toolWindow->GetName());

		InitProjectBrowserStyle();
		InitEditorStyle();
    }

    void EditorStyle::InitProjectBrowserStyle()
    {
		if (!projectBrowserTabView)
		{
			projectBrowserTabView = CreateObject<FTabViewStyle>(this, "ProjectBrowserTabView");
			Add("TabView.ProjectBrowser", projectBrowserTabView);
		}

		projectBrowserTabView->tabItemActiveBackground = Color::RGBA(40, 40, 40);
		projectBrowserTabView->tabItemHoverBackground = Color::RGBA(36, 36, 36);
		projectBrowserTabView->tabItemBackground = Color::RGBA(26, 26, 26);
		projectBrowserTabView->tabItemShape = FRoundedRectangle(5, 5, 0, 0);

    	projectBrowserTabView->containerBackground = projectBrowserTabView->tabItemActiveBackground;
		projectBrowserTabView->containerBorderColor = Color::Clear();
		projectBrowserTabView->containerBorderWidth = 0.0f;

		if (!projectBrowserWindow)
		{
			projectBrowserWindow = CreateObject<FToolWindowStyle>(this, "ProjectBrowserWindow");
			Add(projectBrowserWindow);
		}

		projectBrowserWindow->background = Color::RGBA(26, 26, 26);

		if (!projectBrowserListView)
		{
			projectBrowserListView = CreateObject<FListViewStyle>(this, "ProjectBrowserListView");
			Add("ProjectBrowserWindow.ListView", projectBrowserListView);
		}

		projectBrowserListView->background = Color::RGBA(26, 26, 26);
		projectBrowserListView->borderWidth = 1.0f;
		projectBrowserListView->borderColor = Color::RGBA(56, 56, 56);

		projectBrowserListView->itemBackground = Color::Clear();
		projectBrowserListView->hoveredItemBackground = highlightColor;
		projectBrowserListView->selectedItemBackground = highlightColor;

		projectBrowserListView->itemBorderWidth = 0.0f;
		projectBrowserListView->itemBorderColor = Color::Clear();

    }

    void EditorStyle::InitEditorStyle()
    {
		if (!editorDockspace)
		{
			editorDockspace = CreateObject<EditorDockspaceStyle>(this, "EditorDockspace");
			Add(editorDockspace);
		}

		GetDefaultWidget<EditorDockspace>()
			.Style(this, editorDockspace->GetName());

		if (!editorMinorDockspace)
		{
			editorMinorDockspace = CreateObject<EditorMinorDockspaceStyle>(this, "EditorMinorDockspace");
			Add(editorMinorDockspace);
		}

		GetDefaultWidget<EditorMinorDockspace>()
			.Style(this, editorMinorDockspace->GetName());

		if (!editorDockTab)
		{
			editorDockTab = CreateObject<EditorDockTabStyle>(this, "EditorDockTab");
			Add(editorDockTab);
		}

		editorDockTab->background = Color::RGBA(26, 26, 26);

		GetDefaultWidget<EditorDockTab>()
			.Style(this, editorDockTab->GetName());

		if (!editorMinorDockTab)
		{
			editorMinorDockTab = CreateObject<EditorDockTabStyle>(this, "EditorMinorDockTab");
			Add(editorMinorDockTab);
		}

		GetDefaultWidget<EditorMinorDockTab>()
			.Style(this, editorMinorDockTab->GetName());

		if (!editorToolBar)
		{
			editorToolBar = CreateObject<FStyledWidgetStyle>(this, "EditorToolBar");
			Add(editorToolBar);
		}

		editorToolBar->background = Color::RGBA(36, 36, 36);
		editorToolBar->borderColor = Color::Clear();
		editorToolBar->borderWidth = 0.0f;

		GetDefaultWidget<EditorToolBar>()
			.Style(this, editorToolBar->GetName());

		if (!editorMenuBar)
		{
			editorMenuBar = CreateObject<EditorMenuBarStyle>(this, "EditorMenuBar");
			Add(editorMenuBar);
		}

		editorMenuBar->background = editorToolBar->background;
		editorMenuBar->borderColor = Color::Clear();
		editorMenuBar->borderWidth = 0.0f;

    	editorMenuBar->itemBackground = Color::Clear();
		editorMenuBar->itemHoverBackground = highlightColor;
		editorMenuBar->itemBorderColor = Color::Clear();
		editorMenuBar->itemHoverBorderColor = Color::Clear();
		editorMenuBar->itemPadding = Vec4(2, 1, 2, 1) * 5;
		
		GetDefaultWidget<EditorMenuBar>()
			.Style(this, editorMenuBar->GetName());

    }

} // namespace CE::Editor
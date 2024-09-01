#include "EditorCore.h"

namespace CE::Editor
{

    EditorStyle::EditorStyle()
    {
	    
    }

    void EditorStyle::Initialize()
    {
        if (!button)
        {
            button = CreateObject<FCustomButtonStyle>(this, "Button");
            Add(button);
        }

		button->background = Color::RGBA(56, 56, 56);
		button->hoveredBackground = Color::RGBA(95, 95, 95);
		button->pressedBackground = Color::RGBA(50, 50, 50);
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
			highlightedButton = CreateObject<FCustomButtonStyle>(this, "Button.Highlighted");
			Add(highlightedButton);
		}

		highlightedButton->background = Color::RGBA(0, 112, 224);
		highlightedButton->hoveredBackground = Color::RGBA(14, 134, 255);
		highlightedButton->pressedBackground = Color::RGBA(0, 100, 200);
		highlightedButton->cornerRadius = Vec4(1, 1, 1, 1) * 5;
		highlightedButton->borderColor = Color::RGBA(24, 24, 24);
    	highlightedButton->borderWidth = 1.0f;

		GetDefaultWidget<FScrollBox>()
			.ScrollBarBackground(scrollBarBackground)
			.ScrollBarBrush(scrollBarColor)
			.ScrollBarHoverBrush(scrollBarHoverColor)
			;

		if (!windowCloseButton)
		{
			windowCloseButton = CreateObject<FCustomButtonStyle>(this, "Button.WindowClose");
			Add(windowCloseButton);
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
			windowControlButton = CreateObject<FCustomButtonStyle>(this, "Button.WindowControl");
			Add(windowControlButton);
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
		textInput->editingBorderColor = Color::RGBA(0, 112, 224);
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
		comboBoxItem->hoverBackground = Color::RGBA(0, 112, 224);
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
		menuPopup->itemPadding = Vec4(10, 5, 10, 5);
		menuPopup->itemHoverBackground = Color::RGBA(0, 112, 224);

		GetDefaultWidget<FMenuPopup>()
			.Style(this, menuPopup->GetName());

		if (!menuBar)
		{
			menuBar = CreateObject<FMenuBarStyle>(this, "MenuBar");
			Add(menuBar);
		}

		menuBar->background = Color::Black();
		menuBar->itemPadding = Vec4(5, 5, 5, 5);
		menuBar->itemHoverBackground = Color::RGBA(0, 112, 224);

		GetDefaultWidget<FMenuBar>()
			.Style(this, menuBar->GetName());

    }

} // namespace CE::Editor

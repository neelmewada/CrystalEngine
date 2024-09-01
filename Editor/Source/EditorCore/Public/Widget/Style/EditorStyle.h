#pragma once

namespace CE::Editor
{

	CLASS()
	class EDITORCORE_API EditorStyle : public FStyleSet
	{
		CE_CLASS(EditorStyle, FStyleSet)
	public:

		EditorStyle();

		void Initialize();

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
		Color scrollBarBackground = Color::RGBA(50, 50, 50);

		FIELD()
		Color scrollBarColor = Color::RGBA(255, 255, 255, 100);

		FIELD()
		Color scrollBarHoverColor = Color::RGBA(255, 255, 255, 140);

	};

} // namespace CE::Editor

#include "EditorStyle.rtti.h"
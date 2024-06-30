#pragma once

#include "FusionCore.h"
#include "VulkanRHI.h"

using namespace CE;


namespace RenderingTests
{
	CLASS()
	class TextInputModel : public FDataModel
	{
		CE_CLASS(TextInputModel, FDataModel)
	public:

	private:

		FIELD()
		String m_Text;

		// - Macro Start -

	protected:
		ScriptEvent<void(const String&)> m_TextModified;
		ScriptEvent<void(const String&)> m_TextEdited;

	public:

		const auto& GetText() const { return m_Text; }

		void SetText_Raw(const String& value)
		{
			m_Text = value;
		}

		void SetText_UI(const String& value)
		{
			SetText_Raw(value);
			m_TextEdited(m_Text);
		}

		void SetText(const String& value)
		{
			SetText_Raw(value);
			m_TextModified(m_Text);
		}

		// - Macro End -

	public:
		void ModifyTextInCode()
		{
			SetText(String::Format("Text from code {}", Random::Range(0, 100)));
		}

	};

	CLASS()
	class RenderingTestWidget : public FWindow, public ApplicationMessageHandler
	{
		CE_CLASS(RenderingTestWidget, FWindow)
	public:

		RenderingTestWidget() = default;

		void BuildPopup(FPopup*& outPopup, int index);

		void Construct() override;

		void OnBeforeDestroy() override;

		void OnWindowRestored(PlatformWindow* window) override;
		void OnWindowMaximized(PlatformWindow* window) override;

		FStackBox* rootBox;
		FButton* button;
		FTextInput* textInput;
		FComboBox* comboBox;
		FLabel* buttonLabel;
		FStyledWidget* subWidget;
		FPopup* btnPopup;
		FPopup* nativePopup;

		FImage* maximizeIcon;
		FStyledWidget* borderWidget;

		FTextInput* modelTextInput;
		TextInputModel* model = nullptr;

		int hitCounter = 0;

		FUSION_WIDGET;
	};

}

#include "FusionCoreTest.rtti.h"

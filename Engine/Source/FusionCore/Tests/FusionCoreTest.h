#pragma once

#include "FusionCore.h"
#include "VulkanRHI.h"

using namespace CE;


namespace RenderingTests
{
	CLASS()
	class TextInputModel : public Object
	{
		CE_CLASS(TextInputModel, Object)
	public:

	private:

		FIELD()
		String m_Text;

		FIELD()
		Array<String> m_ComboItems;

		// - Macro Start -

	protected:
		FVoidEvent m_OnTextModified;
		FVoidEvent m_OnTextEdited;

		FVoidEvent m_OnComboItemsModified;
		FVoidEvent m_OnComboItemsEdited;

	public:

		const auto& GetText() const { return m_Text; }

		void SetText_Raw(const String& value)
		{
			m_Text = value;
		}

		void SetText_UI(const String& value)
		{
			SetText_Raw(value);
			m_OnTextEdited();
		}

		void SetText(const String& value)
		{
			SetText_Raw(value);
			m_OnTextModified();
		}

		auto& OnTextModified() { return m_OnTextModified; }
		auto& OnTextEdited() { return m_OnTextEdited; }

		Self& OnTextModified(const FunctionBinding& binding)
		{
			m_OnTextModified.Bind(binding);
			return *this;
		}

		template <typename TLambda>
		Self& OnTextModified(const TLambda& lambda)
		{
			m_OnTextModified.Bind(lambda);
			return *this;
		}

		template <typename TLambda>
		Self& OnTextModified(DelegateHandle& outHandle, const TLambda& lambda)
		{
			outHandle = m_OnTextModified.Bind(lambda);
			return *this;
		}

		const auto& GetComboItems() const { return m_ComboItems; }

		void SetComboItems_Raw(const Array<String>& value)
		{
			m_ComboItems = value;
		}

		void SetComboItems_UI(const Array<String>& value)
		{
			SetComboItems_Raw(value);
			m_OnComboItemsEdited();
		}

		void SetComboItems(const Array<String>& value)
		{
			SetComboItems_Raw(value);
			m_OnComboItemsModified();
		}

		auto& OnComboItemsModified() { return m_OnComboItemsModified; }
		auto& OnComboItemsEdited() { return m_OnComboItemsEdited; }



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
		void OnWindowExposed(PlatformWindow* window) override;

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

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

		TextInputModel()
		{
			m_Text = "[Text]";
		}

		virtual void OnModelPropertyUpdated(const CE::Name& propertyName, Object* modifyingObject) {}

		MODEL_PROPERTY(String, Text);
		MODEL_PROPERTY(Array<String>, ComboItems);
		MODEL_PROPERTY(u64, MemoryFootprint)

	public:

		void ModifyTextInCode()
		{
			SetText(String::Format("Text from code {}", Random::Range(0, 100)));
		}

		void UpdateMemoryFootprint()
		{
			auto app = FusionApplication::Get();
			SetMemoryFootprint(app->ComputeMemoryFootprint());
			String footprint = String::Format("Fusion Memory: {:.2f} MB", GetMemoryFootprint() / 1024.0f / 1024.0f);
			CE_LOG(Info, All, footprint);
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
		FLabel* modelDisplayLabel;
		TextInputModel* model = nullptr;

		int hitCounter = 0;

		FUSION_WIDGET;
	};

}

#include "FusionCoreTest.rtti.h"

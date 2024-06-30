#pragma once

#include "FusionCore.h"
#include "VulkanRHI.h"

using namespace CE;


namespace RenderingTests
{

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

		int hitCounter = 0;

		FUSION_WIDGET;
	};

}

#include "FusionCoreTest.rtti.h"

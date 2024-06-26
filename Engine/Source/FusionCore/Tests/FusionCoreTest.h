#pragma once

#include "FusionCore.h"
#include "VulkanRHI.h"

using namespace CE;


namespace RenderingTests
{

	CLASS()
	class RenderingTestWidget : public FWindow
	{
		CE_CLASS(RenderingTestWidget, FWindow)
	public:

		RenderingTestWidget() = default;

		void Construct() override;

		FStackBox* rootBox;
		FButton* button;
		FTextInput* textInput;
		FLabel* buttonLabel;
		FStyledWidget* subWidget;
		FPopup* btnPopup;

		int hitCounter = 0;

		FUSION_WIDGET;
	};

}

#include "FusionCoreTest.rtti.h"

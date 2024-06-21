#pragma once

#include "FusionCore.h"
#include "VulkanRHI.h"

using namespace CE;


namespace LayoutTests
{
	CLASS()
	class TerminalWidget : public FWidget
	{
		CE_CLASS(TerminalWidget, FWidget)
	public:

		TerminalWidget() = default;

		void Construct() override;

		// - Fusion Fields -


		// - Fusion Properties -

		
		FUSION_WIDGET;
	};

	CLASS()
	class LayoutTestWidget : public FCompoundWidget
	{
		CE_CLASS(LayoutTestWidget, FCompoundWidget)
	public:

		LayoutTestWidget() = default;

		void Construct() override;

		FStackBox* rootBox = nullptr;

		FHorizontalStack* hStack1 = nullptr;
		FHorizontalStack* hStack2 = nullptr;
		FHorizontalStack* hStack3 = nullptr;

		FUSION_WIDGET;
	};

}

namespace RenderingTests
{

	CLASS()
	class RenderingTestWidget : public FCompoundWidget
	{
		CE_CLASS(RenderingTestWidget, FCompoundWidget)
	public:

		RenderingTestWidget() = default;

		void Construct() override;

		FStackBox* rootBox;
		FButton* button;
		FLabel* buttonLabel;
		FStyledWidget* subWidget;

		int hitCounter = 0;

		FUSION_WIDGET;
	};

}

#include "FusionCoreTest.rtti.h"

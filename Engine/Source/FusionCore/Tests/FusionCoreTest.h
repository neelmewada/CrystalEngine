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

		

	};

	CLASS()
	class LayoutTestWidget : public FCompoundWidget
	{
		CE_CLASS(LayoutTestWidget, FCompoundWidget)
	public:

		LayoutTestWidget() = default;

		void Construct() override;

		FStackBox* rootBox = nullptr;

		FStackBox* hStack1 = nullptr;
		FStackBox* hStack2 = nullptr;
		FStackBox* hStack3 = nullptr;
	};

}

#include "FusionCoreTest.rtti.h"

#pragma once

#include "FusionCore.h"
#include "VulkanRHI.h"

using namespace CE;

namespace ConstructTests
{

	CLASS()
	class ComplexWidget : public FCompoundWidget
	{
		CE_CLASS(ComplexWidget, FCompoundWidget)
	public:

		ComplexWidget();

		void Construct() override;

		FStackBox* rootBox = nullptr;

	};
	
}

namespace LayoutTests
{
	CLASS()
	class TerminalWidget : public FWidget
	{
		CE_CLASS(TerminalWidget, FWidget)
	public:

		TerminalWidget() = default;

		void Construct() override;

	};

	CLASS()
	class LayoutTestWidget : public FCompoundWidget
	{
		CE_CLASS(LayoutTestWidget)
	public:

		LayoutTestWidget() = default;

		void Construct() override;

	};

}

#include "FusionCoreTest.rtti.h"

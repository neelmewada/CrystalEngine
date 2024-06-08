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
	
}

#include "FusionCoreTest.rtti.h"

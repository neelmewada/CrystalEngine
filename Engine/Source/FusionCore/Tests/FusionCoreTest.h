#pragma once

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
	class LayoutTestWidget : public FCompoundWidget
	{
		CE_CLASS(LayoutTestWidget, FCompoundWidget)
	public:

		LayoutTestWidget();

		void Construct() override;


	};

	
}


#include "FusionCoreTest.rtti.h"
#include "FusionCoreImpl.h"

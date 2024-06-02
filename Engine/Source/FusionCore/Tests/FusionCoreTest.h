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


#include "FusionCoreTest.rtti.h"

#include "FusionCoreImpl.h"

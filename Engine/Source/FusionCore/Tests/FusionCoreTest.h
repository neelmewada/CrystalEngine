#pragma once

using namespace CE;

namespace ConstructTests
{
	CLASS()
	class SimpleWidget : public FCompoundWidget
	{
		CE_CLASS(SimpleWidget, FCompoundWidget)
	public:

		SimpleWidget();

		void Construct() override;

		FIELD()
		FStackBox* stackBox = nullptr;

		FIELD()
		FNullWidget* first = nullptr;

		FIELD()
		FNullWidget* second = nullptr;
	};

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

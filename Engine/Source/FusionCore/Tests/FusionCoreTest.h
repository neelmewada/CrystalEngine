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
	class TerminalWidget : public FWidget
	{
		CE_CLASS(TerminalWidget, FWidget)
	public:

		TerminalWidget();

		void Construct() override;

		void PrecomputeLayoutSize() override;

	private:

		FIELD()
		Vec2 m_IntrinsicSize = Vec2();

	public:

		FUSION_PROPERTY(IntrinsicSize);

	};

	CLASS()
	class LayoutTestWidget : public FCompoundWidget
	{
		CE_CLASS(LayoutTestWidget, FCompoundWidget)
	public:

		LayoutTestWidget();

		void Construct() override;

		FStackBox* rootBox = nullptr;

		FStackBox* horizontalGroup1 = nullptr;
		FStackBox* horizontalGroup2 = nullptr;
		FStackBox* horizontalGroup3 = nullptr;

	};

	
}


#include "FusionCoreTest.rtti.h"
#include "FusionCoreImpl.h"

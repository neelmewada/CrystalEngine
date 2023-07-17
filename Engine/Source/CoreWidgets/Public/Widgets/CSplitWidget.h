#pragma once

namespace CE::Widgets
{
	CLASS()
	class COREWIDGETS_API CSplitWidget : public CWidget
	{
		CE_CLASS(CSplitWidget, CWidget)
	public:

		CSplitWidget();
		virtual ~CSplitWidget();

		bool IsContainer() override { return true; }

		void Construct() override;

	protected:

		void OnDrawGUI() override;

	};
    
} // namespace CE::Widgets

#include "CSplitWidget.rtti.h"

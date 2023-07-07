#pragma once

namespace CE::Widgets
{

	CLASS()
	class COREWIDGETS_API CSelectableWidget : public CWidget
	{
		CE_CLASS(CSelectableWidget, CWidget)
	public:

		CSelectableWidget();
		virtual ~CSelectableWidget();

		bool IsContainer() override { return true; }

	protected:

		void OnDrawGUI() override;

	};
    
} // namespace CE::Widgets

#include "CSelectableWidget.rtti.h"

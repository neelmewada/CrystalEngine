#pragma once

namespace CE::Widgets
{

	CLASS()
	class CSelectableWidget : public CWidget
	{
		CE_CLASS(CSelectableWidget, CWidget)
	public:

		CSelectableWidget();
		virtual ~CSelectableWidget();

	protected:

		void OnDrawGUI() override;

	};
    
} // namespace CE::Widgets

#include "CSelectableWidget.rtti.h"

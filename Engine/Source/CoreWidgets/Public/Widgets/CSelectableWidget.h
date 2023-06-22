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


	protected:

		virtual void OnDrawGUI() override;

		void HandleEvent(CEvent* event) override;

		FIELD()
		b8 isSelected = false;
	};
    
} // namespace CE::Widgets


#include "CSelectableWidget.rtti.h"

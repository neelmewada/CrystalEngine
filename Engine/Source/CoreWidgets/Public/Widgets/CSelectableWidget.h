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

		inline bool IsSelected() const
		{
			return isSelected;
		}
		
		inline void Select(bool select)
		{
			isSelected = select;
		}

	protected:

		virtual void OnDrawGUI() override;

		void HandleEvent(CEvent* event) override;

		FIELD()
		b8 isSelected = false;
	};
    
} // namespace CE::Widgets


#include "CSelectableWidget.rtti.h"

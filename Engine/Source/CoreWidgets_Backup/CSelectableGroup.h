#pragma once

namespace CE::Widgets
{

	CLASS()
	class COREWIDGETS_API CSelectableGroup : public CStackLayout
	{
		CE_CLASS(CSelectableGroup, CStackLayout)
	public:

		CSelectableGroup();
		virtual ~CSelectableGroup();

	protected:

		void OnSubWidgetAttached(CWidget* widget) override;

		void OnDrawGUI() override;

		void HandleEvent(CEvent* event) override;

	private:

		int selectedIndex = -1;
	};
    
} // namespace CE::Widgets


#include "CSelectableGroup.rtti.h"

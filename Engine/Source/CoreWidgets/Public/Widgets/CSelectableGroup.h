#pragma once

namespace CE::Widgets
{
    CLASS()
	class COREWIDGETS_API CSelectableGroup : public CWidget
	{
		CE_CLASS(CSelectableGroup, CWidget)
	public:

		CSelectableGroup();
		virtual ~CSelectableGroup();

		bool IsContainer() override { return true; }

		inline bool AllowMultiSelection() const { return allowMultiSelection; }
		inline void SetAllowMultiSelection(bool set) { allowMultiSelection = set; }

		bool IsSubWidgetAllowed(ClassType* widgetClass) override;

		u32 GetSelectedItemIndex();
		bool IsItemSelectedAt(u32 index);
		bool SelectItemAt(u32 index);

	protected:

		void OnDrawGUI() override;

		void OnWidgetSelected(CSelectableWidget* selectedWidget);

	protected:

		FIELD()
		b8 allowMultiSelection = false;

		friend class CSelectableWidget;
	};

} // namespace CE::Widgets

#include "CSelectableGroup.rtti.h"

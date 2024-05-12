#pragma once

namespace CE::Widgets
{
	class CTreeWidgetItem;
	class CTreeWidget;

	CLASS()
	class CRYSTALWIDGETS_API CTreeWidgetRow : public CWidget
	{
		CE_CLASS(CTreeWidgetRow, CWidget)
	public:

		CTreeWidgetRow();

		virtual ~CTreeWidgetRow();

		void SetAlternate(bool set);

	private:

		FIELD()
		CTreeWidgetItem* ownerItem = nullptr;

		FIELD()
		CTreeWidget* treeWidget = nullptr;

		friend class CTreeWidgetItem;
		friend class CTreeWidget;
	};

} // namespace CE::Widgets

#include "CTreeWidgetRow.rtti.h"
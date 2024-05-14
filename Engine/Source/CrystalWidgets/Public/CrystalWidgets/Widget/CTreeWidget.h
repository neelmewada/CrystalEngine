#pragma once

namespace CE::Widgets
{
	CLASS()
	class CRYSTALWIDGETS_API CTreeWidget : public CWidget
	{
		CE_CLASS(CTreeWidget, CWidget)
	public:

		CTreeWidget();

		virtual ~CTreeWidget();

		bool IsSubWidgetAllowed(Class* subWidgetClass) override;

		void SetSelectionMode(CItemSelectionMode selectionMode);

		void Select(CTreeWidgetItem* item, bool selectAdditive = false);

	protected:

		void UpdateRows();

		void UpdateSelection();

	private:

		void Construct() override;

		void OnSubobjectAttached(Object* subobject) override;
		void OnSubobjectDetached(Object* subobject) override;

		FIELD()
		CItemSelectionMode selectionMode = CItemSelectionMode::SingleSelection;

		FIELD()
		Array<CTreeWidgetItem*> items{};

		FIELD()
		Array<CTreeWidgetRow*> rows{};

		HashSet<CTreeWidgetItem*> selectedItems{};

		friend class CTreeWidgetItem;
		friend class CTreeWidgetRow;
	};

} // namespace CE::Widgets

#include "CTreeWidget.rtti.h"
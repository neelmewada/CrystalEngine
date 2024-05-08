#pragma once

namespace CE::Widgets
{

	CLASS()
	class CRYSTALWIDGETS_API CListWidget : public CScrollArea
	{
		CE_CLASS(CListWidget, CScrollArea)
	public:

		CListWidget();
		virtual ~CListWidget();

		bool IsSubWidgetAllowed(Class* subWidgetClass) override;

		void Select(CListWidgetItem* item, bool additive = false);

		void Select(int index, bool additive = false);

		void SetSelectionMode(CItemSelectionMode mode);

		const Array<CListWidgetItem*>& GetSelection() const { return selectedItems; }

		// - Signals -

		CE_SIGNAL(OnSelectionChanged);

	protected:

		void UpdateSelectionState();

		void OnItemClicked(CListWidgetItem* item, KeyModifier modifiers);

		void OnSubobjectAttached(Object* subobject) override;
		void OnSubobjectDetached(Object* subobject) override;

		void HandleEvent(CEvent* event) override;

		FIELD()
		Array<CListWidgetItem*> items{};

		FIELD()
		CItemSelectionMode selectionMode = CItemSelectionMode::SingleSelection;

		Array<CListWidgetItem*> selectedItems{};

		friend class CListWidgetItem;
	};

} // namespace CE::Widgets

#include "CListWidget.rtti.h"
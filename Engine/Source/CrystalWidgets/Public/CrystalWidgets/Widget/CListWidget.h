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

		void Select(CListWidgetItem* item);

	protected:

		void OnSubobjectAttached(Object* subobject) override;
		void OnSubobjectDetached(Object* subobject) override;

		void HandleEvent(CEvent* event) override;

		FIELD()
		Array<CListWidgetItem*> items{};

		FIELD()
		CListWidgetItem* selectedItem = nullptr;

	};

} // namespace CE::Widgets

#include "CListWidget.rtti.h"
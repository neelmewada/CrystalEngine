#pragma once

namespace CE::Widgets
{
	class CListWidget;

	CLASS()
	class CRYSTALWIDGETS_API CListWidgetItem : public CWidget
	{
		CE_CLASS(CListWidgetItem, CWidget)
	public:

		CListWidgetItem();
		virtual ~CListWidgetItem();

	protected:

		void HandleEvent(CEvent* event) override;

		FIELD()
		CListWidget* owner = nullptr;

		FIELD()
		int index = -1;

		friend class CListWidget;
	};

} // namespace CE::Widgets

#include "CListWidgetItem.rtti.h"
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

	private:

		void Construct() override;

		void OnSubobjectAttached(Object* subobject) override;
		void OnSubobjectDetached(Object* subobject) override;

		void UpdateRows();

		FIELD()
		Array<CTreeWidgetItem*> items{};

		FIELD()
		Array<CTreeWidgetRow*> rows{};

		friend class CTreeWidgetItem;
	};

} // namespace CE::Widgets

#include "CTreeWidget.rtti.h"
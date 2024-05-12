#pragma once

namespace CE::Widgets
{
	class CTreeWidget;
	class CTreeWidgetRow;

	CLASS()
	class CRYSTALWIDGETS_API CTreeWidgetItem : public CWidget
	{
		CE_CLASS(CTreeWidgetItem, CWidget)
	public:

		CTreeWidgetItem();

		virtual ~CTreeWidgetItem();

		bool IsSubWidgetAllowed(Class* subWidgetClass) override;

		bool IsExpanded() const { return isExpanded; }

		void SetExpanded(bool expanded);

		CTreeWidget* GetTreeWidget();

		const String& GetText() const;

		void SetText(const String& text);

	private:

		void Construct() override;

		void OnSubobjectAttached(Object* subobject) override;

		void OnSubobjectDetached(Object* subobject) override;

	protected:

		void OnPaint(CPaintEvent* paintEvent) override;

		FIELD()
		bool createLabel = true;

		FIELD()
		CLabel* label = nullptr;

	private:

		FIELD()
		CTreeWidget* treeWidget = nullptr;

		FIELD()
		Array<CTreeWidgetItem*> children{};

		FIELD()
		CTreeWidgetRow* row = nullptr;

		FIELD()
		bool isExpanded = false;

		friend class CTreeWidget;
		friend class CTreeWidgetRow;
	};

} // namespace CE::Widgets

#include "CTreeWidgetItem.rtti.h"
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

		bool IsExpandedInHierarchy() const;

		void SetExpanded(bool expanded);

		CTreeWidget* GetTreeWidget();

		const String& GetText() const;

		void SetText(const String& text);

		void AddChild(CTreeWidgetItem* child);

		void SetForceExpanded(bool forceExpanded);

		void* GetInternalDataPtr() const { return internalData; }

		void SetInternalDataPtr(void* ptr) { internalData = ptr; }

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

		void* internalData = nullptr;

		FIELD()
		CTreeWidget* treeWidget = nullptr;

		FIELD()
		Array<CTreeWidgetItem*> children{};

		FIELD()
		CTreeWidgetRow* row = nullptr;

		FIELD()
		CImage* arrowIcon = nullptr;

		FIELD()
		bool isExpanded = false;

		FIELD()
		bool forceExpanded = false;

		friend class CTreeWidget;
		friend class CTreeWidgetRow;
	};

} // namespace CE::Widgets

#include "CTreeWidgetItem.rtti.h"
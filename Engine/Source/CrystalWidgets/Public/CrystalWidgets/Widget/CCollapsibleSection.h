#pragma once

namespace CE::Widgets
{

	CLASS()
	class CRYSTALWIDGETS_API CCollapsibleSection : public CWidget
	{
		CE_CLASS(CCollapsibleSection, CWidget)
	public:

		CCollapsibleSection();

		virtual ~CCollapsibleSection();

		bool IsExpanded() const { return isExpanded; }

		void SetExpanded(bool expanded);

		const String& GetTitle() const { return titleLabel->GetText(); }

		void SetTitle(const String& title);

		CWidget* GetHeader() const { return header; }

		CWidget* GetContent() const { return content; }

	protected:

		void Construct() override;

		FIELD()
		CWidget* header = nullptr;

		FIELD()
		CWidget* content = nullptr;

		FIELD()
		CImage* dropDownArrow = nullptr;

		FIELD()
		CLabel* titleLabel = nullptr;

		FIELD()
		bool isInitiallyExpanded = false;

	private:

		FIELD()
		bool isExpanded = false;

		CE_WIDGET_FRIENDS()
	};

} // namespace CE::Widgets

#include "CCollapsibleSection.rtti.h"
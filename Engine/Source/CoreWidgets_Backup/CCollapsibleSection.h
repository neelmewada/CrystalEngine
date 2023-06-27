#pragma once

namespace CE::Widgets
{

	CLASS()
		class COREWIDGETS_API CCollapsibleSection : public CWidget
	{
		CE_CLASS(CCollapsibleSection, CWidget)
	public:

		CCollapsibleSection();
		virtual ~CCollapsibleSection();

		inline bool IsOpen() const
		{
			return isOpen;
		}

		inline const String& GetTitle() const
		{
			return title;
		}

		inline void SetTitle(const String& title)
		{
			this->title = title;
			internalTitle = String::Format(title + "##{}", GetUuid());
		}

		inline int GetIndent() const { return indentLevel; }
		inline void SetIndent(int indentLevel) { this->indentLevel = indentLevel; }

		bool IsContainer() override { return true; }

	protected:
		void OnDrawGUI() override;

		void HandleEvent(CEvent* event) override;

	private:

		FIELD()
		String title{};

		FIELD()
		bool isOpen = false;

		FIELD()
		int indentLevel = 0;

		String internalTitle{};
	};

} // namespace CE::Widgets

#include "CCollapsibleSection.rtti.h"

#pragma once

namespace CE::Widgets
{
	struct CSSRule
	{
		CSSSelectorList selectorList{};
		CStyle style{};
	};

    CLASS()
	class COREWIDGETS_API CSSStyleSheet : public CStyleSheet
	{
		CE_CLASS(CSSStyleSheet, CStyleSheet)
	public:

		CSSStyleSheet();
		virtual ~CSSStyleSheet();


	private:

		Array<CSSRule> rules{};

		friend class CSSParser;
	};

} // namespace CE::Widgets

#include "CSSStyleSheet.rtti.h"

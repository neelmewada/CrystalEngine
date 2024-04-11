#pragma once

namespace CE::Widgets
{
	struct CSSRule
	{
		CSSSelectorList selectorList{};
		CStyle style{};
	};

	CLASS()
	class CRYSTALWIDGETS_API CSSStyleSheet : public CStyleSheet
	{
		CE_CLASS(CSSStyleSheet, CStyleSheet)
	public:

		virtual CStyle SelectStyle(CWidget* widget, CStateFlag state = CStateFlag::Default, CSubControl subControl = CSubControl::None) override;

		virtual void Clear() override;

	private:

		Array<CSSRule> rules{};

		HashMap<SIZE_T, CStyle> cachedStyle{};

		friend class CSSParser;
	};

} // namespace CE::Widgets

#include "CSSStyleSheet.rtti.h"
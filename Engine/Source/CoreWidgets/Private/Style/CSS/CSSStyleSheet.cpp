#include "CoreWidgets.h"

namespace CE::Widgets
{

	CSSStyleSheet::CSSStyleSheet()
	{

	}

	CSSStyleSheet::~CSSStyleSheet()
	{
		
	}

	Array<CSSRule> CSSStyleSheet::GetMatchingRules(CWidget* widget, CStateFlag state, CSubControl subControl)
	{
		if (widget == nullptr)
			return {};

		Array<CSSRule> result{};

		if (parent != nullptr && parent != this)
		{
			result = parent->GetMatchingRules(widget, state, subControl);
		}

		for (auto& rule : rules)
		{
			if (rule.selectorList.TestWidget(widget, state, subControl))
				result.Add(rule);
		}

		return result;
	}

	void CSSStyleSheet::Clear()
	{
		rules.Clear();
	}
	

} // namespace CE::Widgets


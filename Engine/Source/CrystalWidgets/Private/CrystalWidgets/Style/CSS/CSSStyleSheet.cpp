#include "CrystalWidgets.h"

namespace CE::Widgets
{

	CStyle CSSStyleSheet::SelectStyle(CWidget* widget, CStateFlag state, CSubControl subControl)
	{
		if (widget == nullptr)
			return {};

		CStyle result{};

		if (parent != nullptr && parent != this)
		{
			result.ApplyProperties(parent->SelectStyle(widget, state, subControl));
		}
		else if (parent == nullptr && CApplication::Get()->GetGlobalStyleSheet() != this)
		{
			result.ApplyProperties(CApplication::Get()->GetGlobalStyleSheet()->SelectStyle(widget, state, subControl));
		}

		for (auto& rule : rules)
		{
			if (rule.selectorList.TestWidget(widget, state, subControl))
				result.ApplyProperties(rule.style);
		}

		return result;
	}

	void CSSStyleSheet::Clear()
	{
		rules.Clear();
	}

} // namespace CE::Widgets

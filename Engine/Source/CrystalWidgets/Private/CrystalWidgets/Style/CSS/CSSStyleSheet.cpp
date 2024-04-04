#include "CrystalWidgets.h"

namespace CE::Widgets
{

	CStyle CSSStyleSheet::SelectStyle(CWidget* widget, CStateFlag state, CSubControl subControl)
	{
		if (widget == nullptr)
			return {};

		CStyle result{};

		CStyleSheet* globalStyleSheet = CApplication::Get()->GetGlobalStyleSheet();

		if (parent != nullptr && parent != this)
		{
			result.ApplyProperties(parent->SelectStyle(widget, state, subControl));
		}
		else if (parent == nullptr && globalStyleSheet != this && globalStyleSheet != nullptr)
		{
			result.ApplyProperties(globalStyleSheet->SelectStyle(widget, state, subControl));
		}

		for (auto& rule : rules)
		{
			if (rule.selectorList.TestWidget(widget, state, subControl))
				result.ApplyProperties(rule.style);
		}

		// TODO: Inherit style from parent widget

		return result;
	}

	void CSSStyleSheet::Clear()
	{
		rules.Clear();
	}

} // namespace CE::Widgets

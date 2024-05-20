#include "CrystalWidgets.h"

namespace CE::Widgets
{

	CStyle CSSStyleSheet::SelectStyle(CWidget* widget, CStateFlag state, CSubControl subControl)
	{
		if (widget == nullptr)
			return {};

		if (isDirty)
		{
			cachedStyle.Clear();
			isDirty = false;
		}

		SIZE_T hash = widget->GetName().GetHashValue();
		CombineHash(hash, widget->GetClass()->GetName());

		for (const auto& styleClass : widget->styleClasses)
		{
			CombineHash(hash, styleClass);
		}

		CombineHash(hash, state);
		CombineHash(hash, subControl);

		CWidget* parentWidget = widget->parent;

		while (parentWidget != nullptr && !parentWidget->IsWindow())
		{
			CombineHash(hash, parentWidget->GetName().GetHashValue());
			CombineHash(hash, parentWidget->GetClass()->GetName());

			for (const auto& styleClass : parentWidget->styleClasses)
			{
				CombineHash(hash, styleClass);
			}

			CombineHash(hash, parentWidget->stateFlags);
			CombineHash(hash, parentWidget->subControl);

			parentWidget = parentWidget->parent;
		}

		if (cachedStyle.KeyExists(hash))
		{
			return cachedStyle[hash];
		}

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
			{
				result.ApplyProperties(rule.style);
			}
		}

		cachedStyle[hash] = result;

		return cachedStyle[hash];
	}

	void CSSStyleSheet::Clear()
	{
		rules.Clear();
	}

} // namespace CE::Widgets

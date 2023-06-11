#include "CoreWidgets.h"

namespace CE::Widgets
{

	bool CStyleSelector::TestSelector(CWidget* widget, CStateFlag state, CSubControl subControl) const
	{
		if (widget == nullptr)
			return false;

		for (const auto& rule : selectorRules)
		{
			if ((rule.state & state) != state || rule.subControl != subControl)
				continue;

			if (rule.objectName == "*") // Any selector
			{
				return true;
			}

			if (rule.shortTypeName == widget->GetClass()->GetName() && (rule.objectName.IsEmpty() || rule.objectName == widget->GetName()))
			{
				return true;
			}
		}
		return false;
	}

	void CStyleManager::PushGlobal(CStylePropertyTypeFlags flags)
	{
		globalStyle.Push(flags);
	}

	void CStyleManager::PopGlobal()
	{
		globalStyle.Pop();
	}

} // namespace CE::Widgets



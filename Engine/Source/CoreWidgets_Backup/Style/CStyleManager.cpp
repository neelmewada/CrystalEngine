#include "CoreWidgets.h"

namespace CE::Widgets
{
	CStyleSelector::CStyleSelector()
	{

	}

	CStyleSelector::CStyleSelector(const String& selector)
	{
		selectorRules.Clear();

		Array<String> entries = selector.Split(',');

		for (const auto& entry : entries)
		{
			Rule rule{};
			EntryType type = Entry_Element;

			for (int i = 0; i < entry.GetLength(); i++)
			{
				if (entry[i] == ' ') // whitespace
					continue;
				if (entry[i] == '#')
				{
					type = Entry_Name;
					continue;
				}
				if (entry[i] == '.')
				{
					type = Entry_StyleClass;
					continue;
				}
				else if (entry[i] == ':')
				{
					if (i < entry.GetLength() - 1 && entry[i + 1] == ':')
						type = Entry_SubControl;
					else
						type = Entry_State;
					continue;
				}
				else if (entry[i] == '*')
				{
					rule.objectName = "*";
					rule.shortTypeName = "*";
					break;
				}

				if (type == Entry_Element)
				{
					String name = "";
					while ((String::IsAlphabet(entry[i]) || String::IsNumeric(entry[i])) && i < entry.GetLength())
					{
						name.Append(entry[i]);
						i++;
					}
					i--;

					rule.shortTypeName = name;
				}
				else if (type == Entry_Name)
				{
					String name = "";
					while ((String::IsAlphabet(entry[i]) || String::IsNumeric(entry[i])) && i < entry.GetLength())
					{
						name.Append(entry[i]);
						i++;
					}
					i--;

					rule.objectName = name;
				}
				else if (type == Entry_StyleClass)
				{
					String styleClass = "";
					while ((String::IsAlphabet(entry[i]) || String::IsNumeric(entry[i]) || entry[i] == '-' || entry[i] == '_') && i < entry.GetLength())
					{
						styleClass.Append(entry[i]);
						i++;
					}
					i--;

					rule.styleClass = styleClass;
				}
				else if (type == Entry_State)
				{
					String state = "";
					while ((String::IsAlphabet(entry[i]) || String::IsNumeric(entry[i])) && i < entry.GetLength())
					{
						state.Append(entry[i]);
						i++;
					}
					i--;

					EnumType* stateEnum = GetStaticEnum<CStateFlag>();
					for (int idx = 0; idx < stateEnum->GetConstantsCount(); idx++)
					{
						String constantName = stateEnum->GetConstant(idx)->GetName().GetString();
						if (constantName.ToLower() == state.ToLower())
						{
							rule.state = (CStateFlag)stateEnum->GetConstant(idx)->GetValue();
							break;
						}
					}
				}
				else if (type == Entry_SubControl)
				{
					String subControl = "";
					while ((String::IsAlphabet(entry[i]) || String::IsNumeric(entry[i])) && i < entry.GetLength())
					{
						subControl.Append(entry[i]);
						i++;
					}
					i--;

					EnumType* subControlEnum = GetStaticEnum<CSubControl>();
					for (int idx = 0; idx < subControlEnum->GetConstantsCount(); idx++)
					{
						String constantName = subControlEnum->GetConstant(idx)->GetName().GetString();
						if (constantName.ToLower() == subControl.ToLower())
						{
							rule.subControl = (CSubControl)subControlEnum->GetConstant(idx)->GetValue();
							break;
						}
					}
				}
			}

			if (!rule.objectName.IsEmpty() || rule.shortTypeName.IsValid() || !rule.styleClass.IsEmpty())
				selectorRules.Add(rule);
		}
	}

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

			auto widgetClass = widget->GetClass();

			while (widgetClass != nullptr)
			{
				Name className = widget->GetClass()->GetName();
				String shortName = className.GetLastComponent();

				if ((!rule.shortTypeName.IsValid() || rule.shortTypeName == className || rule.shortTypeName == shortName) &&
					(rule.objectName.IsEmpty() || rule.objectName == widget->GetName()) &&
					(rule.styleClass.IsEmpty() || widget->StyleClassExists(rule.styleClass)))
				{
					return true;
				}

				if (widgetClass->GetSuperClassCount() == 0)
					break;

				widgetClass = widgetClass->GetSuperClass(0);
			}
		}
		return false;
	}

	bool CStyleSelector::operator==(const CStyleSelector& other)
	{
		if (selectorRules.GetSize() != other.selectorRules.GetSize())
			return false;

		for (int i = 0; i < selectorRules.GetSize(); i++)
		{
			const auto& thisRule = selectorRules[i];
			const auto& otherRule = other.selectorRules[i];

			if (thisRule.objectName != otherRule.objectName || thisRule.shortTypeName != otherRule.shortTypeName ||
				thisRule.state != otherRule.state || thisRule.subControl != otherRule.subControl)
				return false;
		}

		return true;
	}

	void CStyleManager::PushGlobal(CStylePropertyTypeFlags flags)
	{
		globalStyle.Push(flags);
	}

	void CStyleManager::PopGlobal()
	{
		globalStyle.Pop();
	}

	CStyle& CStyleManager::AddStyleGroup(const String& selector)
	{
		CStyleGroup group{};
		group.selector = selector;
		styleGroups.Add(group);

		return styleGroups.Top().style;
	}

	CStyle& CStyleManager::GetStyleGroup(const String& selector)
	{
		CStyleSelector search = selector;

		for (auto& styleGroup : styleGroups)
		{
			if (search == styleGroup.selector)
			{
				return styleGroup.style;
			}
		}

		CStyleGroup group{};
		group.selector = search;
		styleGroups.Add(group);
		return styleGroups.Top().style;
	}

} // namespace CE::Widgets



#include "CoreWidgets.h"

namespace CE::Widgets
{

	bool CSSSelector::TestMatch(CWidget* widget, CStateFlag curStates, CSubControl subControl)
	{
		if (widget == nullptr)
			return false;
		if (!primary.IsValid())
			return false;

		if (primary.matches == Any && !HasSecondRule())
			return true;
		
		if (!HasSecondRule())
		{
			return TestMatch(widget, primary, curStates, subControl);
		}

		// Second rule exists
		
		bool secondRuleMatches = TestMatch(widget, secondary, curStates, subControl);
		if (!secondRuleMatches)
			return false;

		switch (relation)
		{
		case Child:
		{
			auto ownerWidget = widget->GetOwner();
			if (ownerWidget != nullptr && TestMatch(ownerWidget, primary, ownerWidget->GetStateFlags(), CSubControl::Any))
				return true;
		}
			break;
		case Descendent:
		{
			auto ownerWidget = widget->GetOwner();
			while (ownerWidget != nullptr)
			{
				if (TestMatch(ownerWidget, primary, ownerWidget->GetStateFlags(), CSubControl::Any))
					return true;
				ownerWidget = ownerWidget->GetOwner();
			}
		}
			break;
		case DirectAdjacent:
		{
			auto ownerWidget = widget->GetOwner();
			if (ownerWidget == nullptr)
				return false;
			int index = ownerWidget->GetSubWidgetIndex(widget);
			if (index < 0 || index >= ownerWidget->GetSubWidgetCount() - 1)
				return false;

			CWidget* directAdjacent = ownerWidget->GetSubWidgetAt(index);

			if (TestMatch(directAdjacent, primary, directAdjacent->GetStateFlags(), CSubControl::Any))
				return true;
		}
		break;
		case IndirectAdjacent:
		{
			auto ownerWidget = widget->GetOwner();
			if (ownerWidget == nullptr)
				return false;
			int index = ownerWidget->GetSubWidgetIndex(widget);
			if (index < 0 || index >= ownerWidget->GetSubWidgetCount() - 1)
				return false;
			index++;

			while (index < ownerWidget->GetSubWidgetCount())
			{
				CWidget* adjacent = ownerWidget->GetSubWidgetAt(index);
				if (TestMatch(adjacent, primary, adjacent->GetStateFlags(), CSubControl::Any))
					return true;
				index++;
			}
		}
			break;
		default:
			break;
		}

		return false;
	}

	bool CSSSelector::TestMatch(CWidget* widget, const MatchCond& rule, CStateFlag curStates, CSubControl subControl)
	{
		if (widget == nullptr)
			return false;
		if (!rule.IsValid())
			return false;

		if (rule.matches == Any)
			return true;

		auto widgetClass = widget->GetClass();
		auto widgetClassName = widgetClass->GetName().GetLastComponent();
		auto widgetId = widget->GetName().GetString();

		while (widgetClass != nullptr && EnumHasAnyFlags(rule.matches, Tag))
		{
			widgetClassName = widgetClass->GetName().GetLastComponent();
			if (widgetClassName == rule.tag)
				break;

			if (widgetClass->GetSuperClassCount() == 0)
				break;
			widgetClass = widgetClass->GetSuperClass(0);
		}

		if (EnumHasAnyFlags(rule.matches, Tag) && rule.tag != widgetClassName)
			return false;
		if (EnumHasAnyFlags(rule.matches, Id) && rule.id != widgetId)
			return false;
		if (EnumHasAnyFlags(rule.matches, Class) && !widget->StyleClassExists(rule.clazz))
			return false;
		if (EnumHasAnyFlags(rule.matches, State) && !EnumHasAllFlags(curStates, rule.states))
			return false;
		if ((EnumHasAnyFlags(rule.matches, SubControl) && rule.subControl != CSubControl::Any && subControl != CSubControl::Any && rule.subControl != subControl) ||
			(!EnumHasAnyFlags(rule.matches, SubControl) && subControl != CSubControl::None && subControl != CSubControl::Any))
			return false;
		
		for (const auto& attribMatchRule : rule.attributeMatches)
		{
			if (!TestAttributeMatch(widget, attribMatchRule))
				return false;
		}

		return true;
	}

	bool CSSSelector::TestAttributeMatch(CWidget* widget, const AttributeMatchCond& rule)
	{
		auto widgetClass = widget->GetClass();
		FieldType* field = widgetClass->FindFieldWithName(rule.attribName);
		if (field == nullptr && !widgetClass->HasAttribute(rule.attribName))
			return false;

		String actualValue = "";
		if (field != nullptr)
			actualValue = field->GetFieldValueAsString(widget);
		else
			actualValue = widgetClass->GetAttribute(rule.attribName).GetStringValue();

		switch (rule.attributeMatch)
		{
		case AttributeExact:
			if (actualValue == rule.attribValue)
				return true;
			break;
		case AttributeContain: 
		case AttributeList:
			if (actualValue.Contains(rule.attribValue))
				return true;
			break;
		case AttributeBegin:
			if (actualValue.StartsWith(rule.attribValue))
				return true;
			break;
		case AttributeEnd:
			if (actualValue.EndsWith(rule.attribValue))
				return true;
			break;
		case AttributeSet:
			if (actualValue == "true" || widgetClass->HasAttribute(rule.attribName))
				return true;
			break;
		case AttributeHyphen:
			if (actualValue == rule.attribValue || actualValue.StartsWith(rule.attribName + "-"))
				return true;
			break;
		default:
			break;
		}

		return false;
	}

    bool CSSSelectorList::TestWidget(CWidget* widget, CStateFlag curStates, CSubControl subControl)
    {
		for (auto& entry : *this)
		{
			if (entry.TestMatch(widget, curStates, subControl))
				return true;
		}
        return false;
    }

	bool CSSSelector::operator==(const CSSSelector& other) const
	{
		return hash == other.hash;
	}

	SIZE_T CSSSelector::CalculateHash()
	{
		hash = 0;
		hash = CalculateHash(primary, hash);
		if (HasSecondRule())
		{
			hash = GetCombinedHash(hash, (int)relation);
			hash = CalculateHash(secondary, hash);
		}

		return hash;
	}

	SIZE_T CSSSelector::CalculateHash(const MatchCond& rule, SIZE_T hash)
	{
		hash = GetCombinedHash(hash, (SIZE_T)rule.matches);

		if (EnumHasAnyFlags(rule.matches, Tag))
			hash = GetCombinedHash(hash, CE::GetHash<String>(rule.tag));
		if (EnumHasAnyFlags(rule.matches, Id))
			hash = GetCombinedHash(hash, CE::GetHash<String>(rule.id));
		if (EnumHasAnyFlags(rule.matches, Class))
			hash = GetCombinedHash(hash, CE::GetHash<String>(rule.clazz));
		if (EnumHasAnyFlags(rule.matches, State))
			hash = GetCombinedHash(hash, CE::GetHash<CStateFlag>(rule.states));
		if (EnumHasAnyFlags(rule.matches, SubControl))
			hash = GetCombinedHash(hash, CE::GetHash<CSubControl>(rule.subControl));

		for (const auto& attribMatchRule : rule.attributeMatches)
		{
			hash = GetCombinedHash(hash, CE::GetHash<String>(attribMatchRule.attribName));
			hash = GetCombinedHash(hash, CE::GetHash((int)attribMatchRule.attributeMatch));
			hash = GetCombinedHash(hash, CE::GetHash<String>(attribMatchRule.attribValue));
		}

		return hash;
	}

} // namespace CE::Widgets

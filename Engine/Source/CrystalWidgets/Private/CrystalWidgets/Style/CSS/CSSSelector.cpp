#include "CrystalWidgets.h"

namespace CE::Widgets
{

	bool CSSSelector::TestMatch(CWidget* widget, const MatchCond& rule, CStateFlag curStates, CSubControl subControl)
	{
		if (widget == nullptr)
			return false;
		if (!rule.IsValid())
			return false;

		if (rule.matches == Any)
			return true;

		ClassType* widgetClass = widget->GetClass();
		Name widgetClassName = widgetClass->GetName().GetLastComponent();
		Name widgetId = widget->GetName();

		while (widgetClass != nullptr && EnumHasAnyFlags(rule.matches, Tag))
		{
			widgetClassName = widgetClass->GetName().GetLastComponent();
			if (rule.tag == widgetClassName)
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

		bool secondRuleMatches = TestMatch(widget, secondary, curStates, subControl);
		if (!secondRuleMatches)
			return false;

		switch (relation)
		{
		case Child:
		{
			auto ownerWidget = widget->parent;
			if (ownerWidget != nullptr && TestMatch(ownerWidget, primary, ownerWidget->stateFlags, CSubControl::Any))
				return true;
		}
		break;
		case Descendent:
		{
			auto ownerWidget = widget->parent;
			while (ownerWidget != nullptr)
			{
				if (TestMatch(ownerWidget, primary, ownerWidget->stateFlags, CSubControl::Any))
					return true;
				ownerWidget = ownerWidget->parent;
			}
		}
		break;
		case DirectAdjacent:
		{
			auto ownerWidget = widget->parent;
			if (ownerWidget == nullptr)
				return false;
			int index = ownerWidget->GetSubWidgetIndex(widget);
			if (index < 0 || index >= ownerWidget->GetSubWidgetCount() - 1)
				return false;

			CWidget* directAdjacent = ownerWidget->GetSubWidget(index);

			if (TestMatch(directAdjacent, primary, directAdjacent->stateFlags, CSubControl::Any))
				return true;
		}
		break;
		case IndirectAdjacent:
		{
			auto ownerWidget = widget->parent;
			if (ownerWidget == nullptr)
				return false;
			int index = ownerWidget->GetSubWidgetIndex(widget);
			if (index < 0 || index >= ownerWidget->GetSubWidgetCount() - 1)
				return false;
			index++;

			while (index < ownerWidget->GetSubWidgetCount())
			{
				CWidget* adjacent = ownerWidget->GetSubWidget(index);
				if (TestMatch(adjacent, primary, adjacent->stateFlags, CSubControl::Any))
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


    bool CSSSelector::TestAttributeMatch(CWidget* widget, const AttributeMatchCond& rule)
    {
		ClassType* widgetClass = widget->GetClass();
		FieldType* field = widgetClass->FindFieldWithName(rule.attribName);
		if (field == nullptr && !widgetClass->HasAttribute(rule.attribName.GetString()))
			return false;

		String actualValue = "";
		if (field != nullptr)
			actualValue = field->GetFieldValueAsString(widget);
		else
			actualValue = widgetClass->GetAttribute(rule.attribName.GetString()).GetStringValue();

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
			if (actualValue == "true" || widgetClass->HasAttribute(rule.attribName.GetString()))
				return true;
			break;
		case AttributeHyphen:
			if (actualValue == rule.attribValue || actualValue.StartsWith(rule.attribName.GetString() + "-"))
				return true;
			break;
		default:
			break;
		}

		return false;
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

	bool CSSSelector::operator==(const CSSSelector& other) const
	{
		return hash == other.hash;
	}

	SIZE_T CSSSelector::CalculateHash(const MatchCond& rule, SIZE_T hash)
	{
		hash = GetCombinedHash(hash, (SIZE_T)rule.matches);

		if (EnumHasAnyFlags(rule.matches, Tag))
			hash = GetCombinedHash(hash, CE::GetHash<Name>(rule.tag));
		if (EnumHasAnyFlags(rule.matches, Id))
			hash = GetCombinedHash(hash, CE::GetHash<Name>(rule.id));
		if (EnumHasAnyFlags(rule.matches, Class))
			hash = GetCombinedHash(hash, CE::GetHash<Name>(rule.clazz));
		if (EnumHasAnyFlags(rule.matches, State))
			hash = GetCombinedHash(hash, CE::GetHash<CStateFlag>(rule.states));
		if (EnumHasAnyFlags(rule.matches, SubControl))
			hash = GetCombinedHash(hash, CE::GetHash<CSubControl>(rule.subControl));

		for (const auto& attribMatchRule : rule.attributeMatches)
		{
			hash = GetCombinedHash(hash, CE::GetHash<Name>(attribMatchRule.attribName));
			hash = GetCombinedHash(hash, CE::GetHash((int)attribMatchRule.attributeMatch));
			hash = GetCombinedHash(hash, CE::GetHash<String>(attribMatchRule.attribValue));
		}

		return hash;
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
    
} // namespace CE::Widgets

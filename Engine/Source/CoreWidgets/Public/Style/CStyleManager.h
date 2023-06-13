#pragma once

namespace CE::Widgets
{

	struct COREWIDGETS_API CStyleSelector
	{
	private:
		struct Rule
		{
			Name shortTypeName{};
			String objectName{};
			CStateFlag state = CStateFlag::Default;
			CSubControl subControl = CSubControl::None;
		};

		enum EntryType
		{
			Entry_None,
			Entry_Element,
			Entry_Name,
			Entry_State,
			Entry_SubControl
		};

	public:

		CStyleSelector();

		CStyleSelector(const String& selector);

		bool TestSelector(CWidget* widget, CStateFlag state = CStateFlag::Default, CSubControl subControl = CSubControl::None) const;

		Array<Rule> selectorRules{};

		bool operator==(const CStyleSelector& other);
	};

	struct CStyleGroup
	{
		CStyleSelector selector{};
		CStyle style{};
	};

	CLASS()
	class COREWIDGETS_API CStyleManager : public Object
	{
		CE_CLASS(CStyleManager, Object)
	public:

		void PushGlobal(CStylePropertyTypeFlags flags = CStylePropertyTypeFlags::All);
		void PopGlobal();

		CStyle& AddStyleGroup(const String& selector);

		// Finds or creates a style group
		CStyle& GetStyleGroup(const String& selector);

		CStyle globalStyle{};

		Array<CStyleGroup> styleGroups{};
	};
    
} // namespace CE::Widgets

#include "CStyleManager.rtti.h"

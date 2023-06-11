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

	public:

		bool TestSelector(CWidget* widget, CStateFlag state = CStateFlag::Default, CSubControl subControl = CSubControl::None) const;

		Array<Rule> selectorRules{};
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

		CStyle globalStyle{};

		Array<CStyleGroup> styleGroups{};
	};
    
} // namespace CE::Widgets

#include "CStyleManager.rtti.h"

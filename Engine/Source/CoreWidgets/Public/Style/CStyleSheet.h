#pragma once

namespace CE::Widgets
{
	struct CSSRule;

	CLASS(Abstract)
	class COREWIDGETS_API CStyleSheet : public Object
	{
		CE_CLASS(CStyleSheet, Object)
	public:

		void OnSubobjectAttached(Object* subObject) override;

		inline bool IsDirty() const { return isDirty; }
		inline void SetDirty(bool set = true) { isDirty = set; }

		inline CStyleSheet* GetParent() const { return parent; }

		virtual Array<CSSRule> GetMatchingRules(CWidget* widget, CStateFlag state = CStateFlag::Default, CSubControl subControl = CSubControl::None) = 0;

		virtual void Clear() = 0;

	protected:

		FIELD()
		CStyleSheet* parent = nullptr;

		FIELD()
		b8 isDirty = false;

		friend class CWidget;
	};
    
} // namespace CE::Widgets

#include "CStyleSheet.rtti.h"

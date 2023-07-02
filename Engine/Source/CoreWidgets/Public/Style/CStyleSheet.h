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

	protected:

		FIELD()
		CStyleSheet* parent = nullptr;

		FIELD()
		b8 isDirty = false;
	};
    
} // namespace CE::Widgets

#include "CStyleSheet.rtti.h"

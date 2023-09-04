#pragma once

namespace CE::Widgets
{
    CLASS()
	class COREWIDGETS_API CCheckbox : public CWidget
	{
		CE_CLASS(CCheckbox, CWidget)
	public:

		CCheckbox();
		virtual ~CCheckbox();

		CE_SIGNAL(OnValueChanged, s8);

		FORCE_INLINE s8 GetRawValue() const { return tristateValue; }
		FORCE_INLINE void SetRawValue(s8 value) { tristateValue = value; }

		FORCE_INLINE bool IsToggled() const { return tristateValue > 0; }
		FORCE_INLINE void ToggleValue(bool value) { tristateValue = value ? 1 : 0; }

	protected:

		Vec2 CalculateIntrinsicContentSize(f32 width, f32 height) override;

		void OnDrawGUI() override;

		FIELD()
		s8 tristateValue = false;
	};
    
} // namespace CE::Widgets

#include "CCheckbox.rtti.h"

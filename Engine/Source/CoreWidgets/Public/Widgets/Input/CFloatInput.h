#pragma once

namespace CE::Widgets
{
    CLASS()
	class COREWIDGETS_API CFloatInput : public CWidget
	{
		CE_CLASS(CFloatInput, CWidget)
	public:

		CFloatInput();
		virtual ~CFloatInput();

	protected:

		void OnDrawGUI() override;

		void HandleEvent(CEvent* event) override;

	public: // Signals

		CE_SIGNAL(OnValueChanged, f32);

	private:

		FIELD()
		f32 value{};

		u32 id = 0;

		f32 min = 0, max = 0;
	};

} // namespace CE::Widgets

#include "CFloatInput.rtti.h"

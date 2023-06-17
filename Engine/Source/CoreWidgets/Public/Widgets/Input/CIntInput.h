#pragma once

namespace CE::Widgets
{
	CLASS()
	class COREWIDGETS_API CIntInput : public CWidget
	{
		CE_CLASS(CIntInput, CWidget)
	public:

		CIntInput();
		virtual ~CIntInput();

	protected:

		void OnDrawGUI() override;

		void HandleEvent(CEvent* event) override;

	public: // Signals

		CE_SIGNAL(OnValueChanged, s64);

	private:

		FIELD()
		s64 value = 0;

		u32 id = 0;

		int min = 0, max = 0;
	};

} // namespace CE::Widgets

#include "CIntInput.rtti.h"

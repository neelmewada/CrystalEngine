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

		inline bool IsChecked() const
		{
			return value > 0;
		}

		inline bool IsUnchecked() const
		{
			return value == 0;
		}

		inline bool IsMixedState() const
		{
			return value < 0;
		}

		inline void SetChecked(bool checked)
		{
			value = checked ? 1 : 0;
		}

		inline void SetMixedState()
		{
			value = -1;
		}

	public: // Signals

		CE_SIGNAL(OnValueChanged, s8);

	protected:
		void OnDrawGUI() override;

		void HandleEvent(CEvent* event) override;

	private:

		FIELD()
		s8 value = 0; // 0 = unchecked | 1 = checked | -1 = mixed state

	};
    
} // namespace CE::Widgets

#include "CCheckbox.rtti.h"

#pragma once

namespace CE::Widgets
{
    CLASS()
	class COREWIDGETS_API CButton : public CWidget
	{
		CE_CLASS(CButton, CE::Widgets::CWidget)
	public:

		CButton();
		virtual ~CButton();

		const String& GetText();
		void SetText(const String& text);

	protected:

		void OnDrawGUI() override;

		void HandleEvent(CEvent* event) override;

	public: // Signals

		CE_SIGNAL(OnButtonClicked);

	private:

		FIELD()
		String text{};
	};

} // namespace CE::Widgets


#include "CButton.rtti.h"

#pragma once

namespace CE::Widgets
{
    
	CLASS()
	class COREWIDGETS_API CButton : public CWidget
	{
		CE_CLASS(CButton, CWidget)
	public:

		CButton();
		virtual ~CButton();

		void Construct() override;

		void SetText(const String& text);
		const String& GetText() const;

		Vec2 CalculateIntrinsicContentSize(f32 width, f32 height) override;

		inline bool IsAlternateStyle() const { return isAlternateStyleButton; }

		void SetAsAlternateStyle(bool set);

	public: // Signals

		CE_SIGNAL(OnButtonClicked);

	protected:

		void OnDrawGUI() override;

		void HandleEvent(CEvent* event) override;

		virtual void OnClicked() {}

	private:

		FIELD()
		String text{};

		FIELD()
		b8 isAlternateStyleButton = false;

		String internalText{};
	};

} // namespace CE::Widgets


#include "CButton.rtti.h"
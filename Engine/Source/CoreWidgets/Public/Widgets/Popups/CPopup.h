#pragma once

namespace CE::Widgets
{
    CLASS()
	class COREWIDGETS_API CPopup : public CWidget
	{
		CE_CLASS(CPopup, CWidget)
	public:

		CPopup();
		virtual ~CPopup();

		// - Getters & Setters -

		FORCE_INLINE const String& GetTitle() const { return title; }
		FORCE_INLINE void SetTitle(const String& title) { this->title = title; }

		// - Public API -

		void Show();
		void Hide();

	protected:

		virtual Vec2 CalculateShowPosition();

		void OnDrawGUI() override;

		bool pushShow = false;

		FIELD()
        String title{};
        
        FIELD()
        b8 isShown = true;

		FIELD()
		b8 showsTitle = true;

		FIELD()
		b8 isScrollDisabled = true;
	};

} // namespace CE::Widgets

#include "CPopup.rtti.h"

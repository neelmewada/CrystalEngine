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

		bool IsContainer() override final { return true; }
		bool IsLayoutCalculationRoot() override final { return true; }

		// - Getters & Setters -

		FORCE_INLINE bool IsShown() const { return isShown; }

		FORCE_INLINE const String& GetTitle() const { return title; }
		FORCE_INLINE void SetTitle(const String& title) { this->title = title; }

		FORCE_INLINE bool IsTitleShown() const { return showsTitle; }
		FORCE_INLINE void SetTitleShown(bool shown) { showsTitle = shown; }

		FORCE_INLINE bool IsCloseButtonShown() const { return closeButtonShown; }
		FORCE_INLINE void SetCloseButtonShown(bool shown) { closeButtonShown = shown; }

		// - Public API -

		void Show();
		void Hide();

	protected:

		Color FetchBackgroundColor(CStateFlag state, CSubControl subControl);

		void OnBeforeComputeStyle() override;
		void OnAfterComputeStyle() override;

		virtual Vec2 CalculateShowPosition();

		void OnDrawGUI() override;

		bool pushShow = false;

		FIELD()
        String title{};
        
        FIELD()
        b8 isShown = true;

		FIELD()
		b8 closeButtonShown = false;

		FIELD()
		b8 showsTitle = true;

		FIELD()
		b8 isScrollDisabled = true;

		Color titleBar{};
		Color titleBarActive{};
	};

} // namespace CE::Widgets

#include "CPopup.rtti.h"

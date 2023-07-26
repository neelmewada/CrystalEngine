#pragma once

namespace CE::Widgets
{
	class CMenu;
    
	CLASS()
	class COREWIDGETS_API CButton : public CWidget
	{
		CE_CLASS(CButton, CWidget)
	public:

		CButton();
		virtual ~CButton();

		bool IsContainer() override { return false; }

		void Construct() override;

		void SetText(const String& text);
		const String& GetText() const;

		void LoadIcon(const String& resourceSearchPath);
		void RemoveIcon();

		Vec2 CalculateIntrinsicContentSize(f32 width, f32 height) override;

		inline bool IsAlternateStyle() const { return isAlternateStyleButton; }

		void SetAsAlternateStyle(bool set);

		inline CMenu* GetPopupMenu() const { return popupMenu; }
		inline void SetPopupMenu(CMenu* menu) { this->popupMenu = menu; }

	public: // Signals

		CE_SIGNAL(OnButtonClicked);

	protected:

		void OnDrawGUI() override;

		void HandleEvent(CEvent* event) override;

		virtual void OnClicked() {}

	private:

		FIELD()
		String text{};

		CTexture icon{};

		FIELD()
		b8 isAlternateStyleButton = false;

		FIELD()
		CMenu* popupMenu = nullptr;

		FIELD()
		b8 openMenuOnLeftClick = true;

		String internalText{};
	};

} // namespace CE::Widgets


#include "CButton.rtti.h"

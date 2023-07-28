#pragma once

namespace CE::Widgets
{

    CLASS()
    class COREWIDGETS_API CMenuItem : public CWidget
    {
        CE_CLASS(CMenuItem, CWidget)
    public:

        CMenuItem();
        virtual ~CMenuItem();

		Vec2 CalculateIntrinsicContentSize(f32 width, f32 height) override;

		virtual void Construct() override;

		bool HasSubMenu();

		bool IsInsideMenuBar();

		// Getters & Setters

		inline const String& GetText() const { return text; }
		inline void SetText(const String& text) { this->text = text; }

		inline bool HasIcon() const { return icon.IsValid(); }

		void LoadIcon(const String& iconSearchPath);

		inline CMenu* GetSubMenu() const { return subMenu; }
		inline void SetSubMenu(CMenu* subMenu) { this->subMenu = subMenu; }

		inline bool IsToggle() const { return itemType == ItemType::Toggle; }
		inline bool IsRadio() const { return itemType == ItemType::Radio; }

		bool HasAnySiblingWithToggleOrRadio();

		inline bool IsToggled() const { return isToggled; }
		inline bool IsRadioSelected() const { return isToggled; }

		void SetAsDefaultType();
		void SetAsToggle();
		void SetAsRadio(const String& radioGroupName);

		void SetToggleValue(bool value);
		void SetRadioValue(bool value = true);

		CE_SIGNAL(OnMenuItemClicked, CMenuItem*);

    protected:

		void OnBeforeComputeStyle() override;

		virtual void OnClicked();

        void OnDrawGUI() override;

		void HandleEvent(CEvent* event) override;

		FIELD()
		String text = "";

		CTexture icon{};

		FIELD()
		CMenu* subMenu = nullptr;

		enum class ItemType { Default, Toggle, Radio };

		ItemType itemType = ItemType::Default;

		FIELD()
		Name radioGroupName = "";

		FIELD()
		b8 isToggled = false;

		b8 siblingExistsWithRadioOrToggle = false;

		GUI::GuiStyleState toggleOrRadioStyleState{};
    };

	CLASS()
	class COREWIDGETS_API CMenuItemHeader : public CWidget
	{
		CE_CLASS(CMenuItemHeader, CWidget)
	public:

		CMenuItemHeader();
		virtual ~CMenuItemHeader();

		Vec2 CalculateIntrinsicContentSize(f32 width, f32 height) override;

		inline const String& GetTitle() const { return title; }
		inline void SetTitle(const String& title) { this->title = title; }

	protected:

		void OnDrawGUI() override;

		FIELD()
		String title = "Header";
	};
    
} // namespace CE

#include "CMenuItem.rtti.h"

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

		void LoadIcon(const String& iconSearchPath);

		inline CMenu* GetSubMenu() const { return subMenu; }
		inline void SetSubMenu(CMenu* subMenu) { this->subMenu = subMenu; }

		inline bool IsToggleable() const { return isToggleable; }
		inline void SetToggleable(bool toggleable) 
		{ 
			this->isToggleable = toggleable;
			SetNeedsStyle();
			SetNeedsLayout();
		}

		inline bool IsToggled() const { return toggleValue; }
		inline void SetToggled(bool set) { toggleValue = set; }

		CE_SIGNAL(OnMenuItemClicked, CMenuItem*);

    protected:

		virtual void OnClicked();

        void OnDrawGUI() override;

		void HandleEvent(CEvent* event) override;

		FIELD()
		String text = "";

		CTexture icon{};

		FIELD()
		CMenu* subMenu = nullptr;

		FIELD()
		b8 isToggleable = false;

		FIELD()
		b8 toggleValue = false;
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

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

		inline CMenu* GetSubMenu() const { return subMenu; }
		inline void SetSubMenu(CMenu* subMenu) { this->subMenu = subMenu; }

    protected:

        void OnDrawGUI() override;

		void HandleEvent(CEvent* event) override;

		FIELD()
		String text = "";

		FIELD()
		CMenu* subMenu = nullptr;
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

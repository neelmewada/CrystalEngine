#pragma once

namespace CE::Widgets
{

    CLASS(Config)
    class COREWIDGETS_API CMenuItem : public CWidget
    {
        CE_CLASS(CMenuItem, CWidget)
    public:

        CMenuItem();
        virtual ~CMenuItem();

		Vec2 CalculateIntrinsicContentSize(f32 width, f32 height) override;

		virtual void Construct() override;

		bool HasSubMenu();

		// Getters & Setters

		inline const String& GetText() const { return text; }
		inline void SetText(const String& text) { this->text = text; }

    protected:

		void OnSubobjectAttached(Object* subobject) override;

        virtual void OnDrawGUI() override;

		void HandleEvent(CEvent* event) override;

		FIELD()
		String text = "";

		FIELD()
		CMenu* subMenu = nullptr;
    };
    
} // namespace CE

#include "CMenuItem.rtti.h"

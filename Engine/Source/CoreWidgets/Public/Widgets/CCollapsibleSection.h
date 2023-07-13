#pragma once

namespace CE::Widgets
{
	CLASS()
	class COREWIDGETS_API CCollapsibleSection : public CWidget
	{
		CE_CLASS(CCollapsibleSection, CWidget)
	public:

		CCollapsibleSection();
		virtual ~CCollapsibleSection();

		void Construct() override;

		bool IsContainer() override { return true; }

		inline const String& GetTitle() const { return title; }
		inline void SetTitle(const String& title) { this->title = title; }

		inline bool IsCollapsed() const { return isCollapsed; }
		void SetCollapsed(bool collapsed);

	protected:

		void OnSubobjectAttached(Object* subobject) override;

		Vec2 CalculateIntrinsicContentSize(f32 width, f32 height) override;

		void OnAfterComputeStyle() override;

		void OnDrawGUI() override;

		// Fields

		GUI::GuiStyleState headerStyleState{};
		CStateFlag headerState = CStateFlag::Default;
		bool hovered = false, leftMouseHeld = false;

		Vec4 headerPadding{};

		u32 internalId = 0;

		FIELD()
		String title = "";

		FIELD()
		b8 isCollapsed = false;
	};
    
} // namespace CE::Widgets

#include "CCollapsibleSection.rtti.h"

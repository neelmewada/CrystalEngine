#include "CoreWidgets.h"

namespace CE::Widgets
{

	CButton::CButton()
	{
		isInteractable = true;
		SetText("Button");
	}

	CButton::~CButton()
	{

	}

	void CButton::Construct()
	{
		Super::Construct();

		Super::subControl = isAlternateStyleButton ? CSubControl::Alternate : CSubControl::None;
	}

	void CButton::SetText(const String& text)
	{
		this->text = text;
		internalText = String::Format(text + "###{}", GetUuid());
	}

	const String& CButton::GetText() const
	{
		return text;
	}

    void CButton::OnBeforeComputeStyle()
    {
        Super::OnBeforeComputeStyle();
        
        CStyle style = stylesheet->SelectStyle(this, CStateFlag::Default, CSubControl::Icon);
        CStyleValue bgImageValue = style.properties[CStylePropertyType::BackgroundImage];
        if (bgImageValue.IsValid() && bgImageValue.IsString())
        {
            icon = GetStyleManager()->SearchImageResource(bgImageValue.string);
        }
    }

	Vec2 CButton::CalculateIntrinsicContentSize(f32 width, f32 height)
	{
		f32 iconSize = 0;
		if (icon.IsValid())
			iconSize = 16;
		return GUI::CalculateTextSize(text) + Vec2(10 + iconSize, 5);
	}

	void CButton::SetAsAlternateStyle(bool set)
	{
		isAlternateStyleButton = set;
		subControl = isAlternateStyleButton ? CSubControl::Alternate : CSubControl::None;
		SetNeedsStyle();
	}

    void CButton::OnDrawGUI()
	{
		Vec4 rect = GetComputedLayoutRect();
		Vec4 padding = GetComputedLayoutPadding();

		f32 iconOffset = 0;
		if (icon.IsValid())
			iconOffset = 10;

		DrawShadow(defaultStyleState);

		bool hovered = false, held = false;
		bool pressed = GUI::Button(rect, internalText, defaultStyleState, hovered, held, padding + Rect(iconOffset, 0, 0, 0));
		PollEvents();

		GUI::PushChildCoordinateSpace(rect);

		if (icon.IsValid())
		{
			constexpr f32 iconSize = 14;
			f32 centerY = (rect.max.y - rect.min.y) / 2;
			Rect iconRect{ padding.left, centerY - iconSize / 2,
				padding.left + iconSize, centerY + iconSize / 2 };
			GUI::Image(iconRect, icon.id, {});
		}

		for (auto child : attachedWidgets)
		{
			child->Render();
		}

		GUI::PopChildCoordinateSpace();

		if (pressed)
		{
			if (openMenuOnLeftClick && popupMenu != nullptr)
			{
				popupMenu->Show(this);
			}

			OnClicked();
			emit OnButtonClicked();
		}
	}

	void CButton::HandleEvent(CEvent* event)
	{

		Super::HandleEvent(event);
	}

} // namespace CE::Widgets

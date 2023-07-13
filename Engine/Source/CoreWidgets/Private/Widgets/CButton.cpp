#include "CoreWidgets.h"

namespace CE::Widgets
{

	CButton::CButton()
	{
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

	Vec2 CButton::CalculateIntrinsicContentSize(f32 width, f32 height)
	{
		return GUI::CalculateTextSize(text) + Vec2(10, 5);
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

		bool hovered = false, held = false;
		bool pressed = GUI::Button(rect, internalText, defaultStyleState, hovered, held, padding);

		PollBasicMouseEvents(hovered, held || pressed, this->stateFlags);

		GUI::PushChildCoordinateSpace(rect);

		for (auto child : attachedWidgets)
		{
			child->RenderGUI();
		}

		GUI::PopChildCoordinateSpace();

		if (pressed)
		{
			OnClicked();
			emit OnButtonClicked();
		}
	}

	void CButton::HandleEvent(CEvent* event)
	{

		Super::HandleEvent(event);
	}

} // namespace CE::Widgets

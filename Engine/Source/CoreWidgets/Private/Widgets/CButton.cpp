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

		auto styleMatch = stylesheet->SelectStyle(this);

		Vec4 padding{};
		padding.left = YGNodeLayoutGetPadding(node, YGEdgeLeft);
		padding.top = YGNodeLayoutGetPadding(node, YGEdgeTop);
		padding.right = YGNodeLayoutGetPadding(node, YGEdgeRight);
		padding.bottom = YGNodeLayoutGetPadding(node, YGEdgeBottom);

		bool hovered = false, held = false;
		bool pressed = GUI::Button(rect, internalText, defaultStyleState, hovered, held, padding);

		HandleBasicMouseEvents(hovered, held || pressed);

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

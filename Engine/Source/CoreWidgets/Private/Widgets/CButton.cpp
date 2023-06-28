#include "CoreWidgets.h"

namespace CE::Widgets
{

	CButton::CButton()
	{

	}

	CButton::~CButton()
	{

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

	void CButton::OnDrawGUI()
	{
		auto top = YGNodeLayoutGetTop(node);
		auto left = YGNodeLayoutGetLeft(node);
		auto width = YGNodeLayoutGetWidth(node);
		auto height = YGNodeLayoutGetHeight(node);

		Vec2 rectPos = Vec2(left, top);
		Vec2 rectSize = Vec2(width, height);
		Vec4 rect = Vec4(rectPos, rectPos + rectSize);

		Vec4 padding{};
		padding.left = YGNodeLayoutGetPadding(node, YGEdgeLeft);
		padding.top = YGNodeLayoutGetPadding(node, YGEdgeTop);
		padding.right = YGNodeLayoutGetPadding(node, YGEdgeRight);
		padding.bottom = YGNodeLayoutGetPadding(node, YGEdgeBottom);

		bool pressed = GUI::Button(rect, internalText, defaultStyleState, hoveredStyleState, pressedStyleState, padding);
		PollEvents();

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

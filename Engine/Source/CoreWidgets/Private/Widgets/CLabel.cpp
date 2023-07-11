
#include "CoreWidgets.h"

namespace CE::Widgets
{

    CLabel::CLabel()
    {
        YGNodeSetNodeType(node, YGNodeTypeText);
    }

    CLabel::~CLabel()
    {
        
    }

    void CLabel::SetText(const String& text)
    {
        this->text = text;
		SetNeedsStyle();
		SetNeedsLayout();
    }

    const String& CLabel::GetText() const
    {
        return text;
    }

	Vec2 CLabel::CalculateIntrinsicContentSize(f32 width, f32 height)
	{
		return GUI::CalculateTextSize(text) + Vec2(6, 3);
	}

    void CLabel::OnDrawGUI()
    {
		Vec4 rect = GetComputedLayoutRect();

		GUI::GuiStyleState* curState = &defaultStyleState;

		if (IsLeftMouseHeld() && IsHovered()) // Pressed
		{
			//curState = &pressedStyleState;
		}
		else if (IsHovered()) // Hovered
		{
			//curState = &hoveredStyleState;
		}

		GUI::SetCursorPos(rect.min);
		auto windowPos = GUI::GetWindowPos();

		DrawBackground(*curState);

		if (invisibleButtonId.IsEmpty())
		{
			invisibleButtonId = String::Format("CLabel_InvisibleButton##{}", GetUuid());
		}

		if (IsInteractable())
		{
			GUI::InvisibleButton(rect, GetUuid());
			PollEvents();
		}

		GUI::Text(rect, text, *curState);
    }

	void CLabel::HandleEvent(CEvent* event)
	{
		Super::HandleEvent(event);
	}
    
} // namespace CE::Widgets


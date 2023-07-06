
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
		auto top = YGNodeLayoutGetTop(node);
		auto left = YGNodeLayoutGetLeft(node);
		auto width = YGNodeLayoutGetWidth(node);
		auto height = YGNodeLayoutGetHeight(node);
		
		Vec2 rectPos = Vec2(left, top);
		Vec2 rectSize = Vec2(width, height);
		Vec4 rect = Vec4(rectPos, rectPos + rectSize);

		GUI::GuiStyleState* curState = &defaultStyleState;

		if (IsLeftMouseHeld() && IsHovered()) // Pressed
		{
			//curState = &pressedStyleState;
		}
		else if (IsHovered()) // Hovered
		{
			//curState = &hoveredStyleState;
		}

		GUI::SetCursorPos(rectPos);
		auto windowPos = GUI::GetWindowPos();

		if (curState->background.a > 0)
		{
			GUI::FillRect(GUI::WindowRectToGlobalRect(rect), curState->background, curState->borderRadius);
		}
		if (curState->borderThickness > 0 && curState->borderColor.a > 0)
		{
			GUI::DrawRect(GUI::WindowRectToGlobalRect(rect), curState->borderColor, curState->borderRadius, curState->borderThickness);
		}

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


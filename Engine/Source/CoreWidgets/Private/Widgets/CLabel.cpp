
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
		float wrapWidth = -1.0f;
		if (wordWrap == CWordWrap::BreakWord && !isnan(width) && width > 0)
			wrapWidth = width;

		CFontManager::Get().PushFont(defaultStyleState.fontSize, defaultStyleState.fontName);
		auto textSize = GUI::CalculateTextSize(text, wrapWidth);
		CFontManager::Get().PopFont();

		if (!isnan(height) && textSize.y > height)
			textSize.y = height;
		Vec2 extra = Vec2(6, 3);
		return textSize + extra;
	}

	void CLabel::OnAfterComputeStyle()
	{
		Super::OnAfterComputeStyle();

		if (computedStyle.properties.KeyExists(CStylePropertyType::WordWrap))
		{
			wordWrap = (CWordWrap)computedStyle.properties[CStylePropertyType::WordWrap].enumValue.x;
		}
	}

	void CLabel::OnDrawGUI()
    {
		Vec4 rect = GetComputedLayoutRect();

		GUI::GuiStyleState* curState = &defaultStyleState;

		//GUI::SetCursorPos(rect.min);
		auto windowPos = GUI::GetWindowPos();

		if (IsInteractable())
		{
			if (GUI::InvisibleButton(rect, GetUuid()))
			{
				emit OnTextClicked();
			}
			PollEvents();
		}

		if (wordWrap == CWordWrap::Ellipsis || wordWrap == CWordWrap::Inherited)
		{
			GUI::TextEllipsis(rect, text, *curState);
		}
		else if (wordWrap == CWordWrap::Clip)
		{
			GUI::TextClipped(rect, text, *curState);
		}
		else if (wordWrap == CWordWrap::BreakWord)
		{
			GUI::TextWrapped(rect, text, *curState);
		}
		else // Normal
		{
			GUI::Text(rect, text, *curState);
		}
    }

	void CLabel::HandleEvent(CEvent* event)
	{
		Super::HandleEvent(event);
	}
    
} // namespace CE::Widgets


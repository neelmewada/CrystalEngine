
#include "CoreWidgets.h"

namespace CE::Widgets
{

    CLabel::CLabel()
    {
		
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

	Vec2 CLabel::CalculateIntrinsicContentSize()
	{
		return GUI::CalculateTextSize(text);
	}

	void CLabel::UpdateStyle()
	{
		
	}

    void CLabel::OnDrawGUI()
    {
		GUI::Text(text);

		/*style.Push();

        Vec4 padding{};
        Vec4 borderRadius{};
		Vec2 preferredSize{};
		Vec2 minSize{};
		Vec2 maxSize{};
		GUI::TextAlign align = GUI::TextAlign_MiddleCenter;
        
        bool hasPadding = false;
		

		if (minSize.x > 0 && preferredSize.x < minSize.x)
			preferredSize.x = minSize.x;
		if (minSize.y > 0 && preferredSize.y < minSize.y)
			preferredSize.y = minSize.y;
		if (maxSize.x > 0 && preferredSize.x > maxSize.x)
			preferredSize.x = maxSize.x;
		if (maxSize.y > 0 && preferredSize.y > maxSize.y)
			preferredSize.y = maxSize.y;

		bool fixedSize = preferredSize.x > 0 || preferredSize.y > 0;

        
		if (padding.x > 0 || padding.y > 0 || padding.z > 0 || padding.w > 0)
			hasPadding = true;
		else
			hasPadding = false;
        
        if (hasPadding)
        {
            Vec2 size = GUI::CalculateTextSize(text);
            Vec2 pos = GUI::GetCursorScreenPos();
            
            if (background != nullptr)
            {
				auto* bgColorValue = background;
				if (IsLeftMouseHeld() && IsHovered() && backgroundPressed != nullptr)
				{
					bgColorValue = backgroundPressed;
				}
				else if (IsHovered() && backgroundHovered != nullptr)
				{
					bgColorValue = backgroundHovered;
				}

				if (bgColorValue->valueType == CStyleValue::Type_Color)
				{
					const Color& color = bgColorValue->color;
					GUI::FillRect(Vec4(pos.x, pos.y, pos.x + size.x + padding.x + padding.z,
						pos.y + size.y + padding.y + padding.w), color, borderRadius);
				}
				else if (bgColorValue->valueType == CStyleValue::Type_Gradient)
				{
					Gradient gradient = bgColorValue->gradient;
					GUI::FillRect(Vec4(pos.x, pos.y, pos.x + size.x + padding.x + padding.z,
						pos.y + size.y + padding.y + padding.w), gradient, borderRadius);
				}
            }
            
            Vec2 cursor = GUI::GetCursorPos();
            if (invisibleButtonId.IsEmpty())
            {
                invisibleButtonId = String::Format("CLabel_InvisibleButton##{}", GetUuid());
            }

            GUI::InvisibleButton(invisibleButtonId, size + Vec2(padding.x + padding.z, padding.y + padding.w));
			PollEvents();
			
            Vec2 finalCursorPos = GUI::GetCursorPos();
            GUI::SetCursorPos(cursor + Vec2(padding.x, padding.y));
			if (fixedSize)
			{
				GUI::Text(text, preferredSize, align);
			}
			else
			{
				GUI::Text(text, {}, align);
			}
            GUI::SetCursorPos(finalCursorPos);
        }
        else
        {
            if (background != nullptr)
            {
                Vec2 size = GUI::CalculateTextSize(text);
                Vec2 pos = GUI::GetCursorScreenPos();
				
				auto* bgColorValue = background;
				if (IsLeftMouseHeld() && IsHovered() && backgroundPressed != nullptr)
				{
					bgColorValue = backgroundPressed;
				}
				else if (IsHovered() && backgroundHovered != nullptr)
				{
					bgColorValue = backgroundHovered;
				}

				if (bgColorValue->valueType == CStyleValue::Type_Color)
				{
					const Color& color = bgColorValue->color;
					GUI::FillRect(Vec4(pos.x, pos.y, pos.x + size.x, pos.y + size.y), color, borderRadius);
				}
				else if (bgColorValue->valueType == CStyleValue::Type_Gradient)
				{
					Gradient gradient = bgColorValue->gradient;
					GUI::FillRect(Vec4(pos.x, pos.y, pos.x + size.x, pos.y + size.y), gradient, borderRadius);
				}
            }
            
			auto cursorPos = GUI::GetCursorPos();
			if (fixedSize)
			{
				GUI::Text(text, preferredSize, align);
			}
			else
			{
				GUI::Text(text, {}, align);
			}
			cursorPos = GUI::GetCursorPos();
			PollEvents();
        }

		style.Pop();*/
    }

	void CLabel::HandleEvent(CEvent* event)
	{
		Super::HandleEvent(event);
	}
    
} // namespace CE::Widgets



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

    const String& CLabel::GetText()
    {
        return text;
    }

	Vec2 CLabel::CalculateEstimateSize()
	{
		Vec4 padding{};
		Vec2 size = GUI::CalculateTextSize(text);

		for (auto& [property, array] : style.styleMap)
		{
			for (auto& styleValue : array)
			{
				if (property == CStylePropertyType::Padding && styleValue.state == CStateFlag::Default)
				{
					padding = styleValue.vector;
				}
			}
		}

		size = Vec2(size.x + padding.x + padding.z, size.y + padding.y + padding.w);

		return size;
	}

    void CLabel::OnDrawGUI()
    {
		style.Push();

        Vec4 padding{};
        Vec4 borderRadius{};
        
        bool hasPadding = false;
		CStyleValue* background = nullptr;
		CStyleValue* backgroundHovered = nullptr;
		CStyleValue* backgroundPressed = nullptr;

		for (auto& [property, array] : style.styleMap)
		{
			for (auto& styleValue : array)
			{
				if (property == CStylePropertyType::Padding && styleValue.state == CStateFlag::Default)
				{
					hasPadding = true;
					padding = styleValue.vector;
				}
				else if (property == CStylePropertyType::Background)
				{
					if ((styleValue.state & CStateFlag::Hovered) != 0)
					{
						backgroundHovered = &styleValue;
					}
					if ((styleValue.state & CStateFlag::Pressed) != 0)
					{
						backgroundPressed = &styleValue;
					}
					if (styleValue.state == CStateFlag::Default)
					{
						background = &styleValue;
					}
				}
				else if (property == CStylePropertyType::BorderRadius && styleValue.state == CStateFlag::Default)
				{
					borderRadius = styleValue.vector;
				}
			}
		}

        
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
            GUI::Text(text);
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
            
            GUI::Text(text);
			PollEvents();
        }

		style.Pop();
    }

	void CLabel::HandleEvent(CEvent* event)
	{
		Super::HandleEvent(event);
	}
    
} // namespace CE::Widgets



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
				if (property == CStylePropertyType::Padding && styleValue.state == CStateFlag::Default && styleValue.subControl == CSubControl::None)
				{
					padding = styleValue.vector;
				}
			}
		}

		size = Vec2(size.x + padding.x + padding.z, size.y + padding.y + padding.w);

		return size;
	}

	void CLabel::UpdateStyle()
	{

	}

    void CLabel::OnDrawGUI()
    {
		style.Push();

        Vec4 padding{};
        Vec4 borderRadius{};
		Vec2 preferredSize{};
		Vec2 minSize{};
		Vec2 maxSize{};
        
        bool hasPadding = false;
		CStyleValue* background = nullptr;
		CStyleValue* backgroundHovered = nullptr;
		CStyleValue* backgroundPressed = nullptr;

		for (auto& [property, array] : style.styleMap)
		{
			for (auto& styleValue : array)
			{
				if (property == CStylePropertyType::Padding && styleValue.state == CStateFlag::Default && styleValue.subControl == CSubControl::None)
				{
					hasPadding = true;
					padding = styleValue.vector;
				}
				else if (property == CStylePropertyType::Background && styleValue.subControl == CSubControl::None)
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
				else if (property == CStylePropertyType::BorderRadius && styleValue.state == CStateFlag::Default && styleValue.subControl == CSubControl::None)
				{
					borderRadius = styleValue.vector;
				}
				if (property == CStylePropertyType::Width)
				{
					if (styleValue.state == CStateFlag::Default)
					{
						if (!styleValue.isPercent)
							preferredSize.x = styleValue.single;
						else
							preferredSize.x = styleValue.single / 100.0f * GUI::GetContentRegionAvailableSpace().x;
					}
				}
				else if (property == CStylePropertyType::Height)
				{
					if (styleValue.state == CStateFlag::Default)
					{
						if (!styleValue.isPercent)
							preferredSize.y = styleValue.single;
						else
							preferredSize.y = styleValue.single / 100.0f * GUI::GetContentRegionAvailableSpace().y;
					}
				}
				else if (property == CStylePropertyType::MaxWidth)
				{
					if (styleValue.state == CStateFlag::Default)
					{
						if (!styleValue.isPercent)
							maxSize.x = styleValue.single;
						else
							maxSize.x = styleValue.single / 100.0f * GUI::GetContentRegionAvailableSpace().x;
					}
				}
				else if (property == CStylePropertyType::MaxHeight)
				{
					if (styleValue.state == CStateFlag::Default)
					{
						if (!styleValue.isPercent)
							maxSize.y = styleValue.single;
						else
							maxSize.y = styleValue.single / 100.0f * GUI::GetContentRegionAvailableSpace().y;
					}
				}
				else if (property == CStylePropertyType::MinWidth)
				{
					if (styleValue.state == CStateFlag::Default)
					{
						if (!styleValue.isPercent)
							minSize.x = styleValue.single;
						else
							minSize.x = styleValue.single / 100.0f * GUI::GetContentRegionAvailableSpace().x;
					}
				}
				else if (property == CStylePropertyType::MinHeight)
				{
					if (styleValue.state == CStateFlag::Default)
					{
						if (!styleValue.isPercent)
							minSize.y = styleValue.single;
						else
							minSize.y = styleValue.single / 100.0f * GUI::GetContentRegionAvailableSpace().y;
					}
				}
			}
		}

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
				GUI::Text(text, preferredSize);
			}
			else
			{
				GUI::Text(text);
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
				GUI::Text(text, preferredSize);
			}
			else
			{
				GUI::Text(text);
			}
			cursorPos = GUI::GetCursorPos();
			PollEvents();
        }

		style.Pop();
    }

	void CLabel::HandleEvent(CEvent* event)
	{
		Super::HandleEvent(event);
	}
    
} // namespace CE::Widgets


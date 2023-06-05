
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

    void CLabel::OnDrawGUI()
    {
        Vec4 padding{};
        Vec4 borderRadius{};
        
        bool hasPadding = false;
        bool hasBackground = style.styles.KeyExists(CStyleVariable::BackgroundColor);
        if (style.styles.KeyExists(CStyleVariable::BorderRadius))
        {
            borderRadius = style.styles[CStyleVariable::BorderRadius].vector;
        }
        
        if (style.styles.KeyExists(CStyleVariable::Padding))
        {
            padding = style.styles[CStyleVariable::Padding].vector;
        }
        
        if (padding.x > 0 || padding.y > 0 || padding.z > 0 || padding.w > 0)
            hasPadding = true;
        
        if (hasPadding)
        {
            Vec2 size = GUI::CalculateTextSize(text);
            Vec2 pos = GUI::GetCursorScreenPos();
            
            if (hasBackground)
            {
				auto& bgColorValue = style.styles[CStyleVariable::BackgroundColor];
				if (bgColorValue.isColor)
				{
					const Color& color = bgColorValue.color;
					GUI::FillRect(Vec4(pos.x, pos.y, pos.x + size.x + padding.x + padding.z,
						pos.y + size.y + padding.y + padding.w), color, borderRadius);
				}
				else if (bgColorValue.isGradient)
				{
					const Gradient& gradient = bgColorValue.gradient;
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
            if (hasBackground)
            {
                Vec2 size = GUI::CalculateTextSize(text);
                Vec2 pos = GUI::GetCursorScreenPos();
				auto& bgColorValue = style.styles[CStyleVariable::BackgroundColor];
				if (bgColorValue.isColor)
				{
					const Color& color = bgColorValue.color;
					GUI::FillRect(Vec4(pos.x, pos.y, pos.x + size.x, pos.y + size.y), color, borderRadius);
				}
				else if (bgColorValue.isGradient)
				{
					const Gradient& gradient = bgColorValue.gradient;
					GUI::FillRect(Vec4(pos.x, pos.y, pos.x + size.x, pos.y + size.y), gradient, borderRadius);
				}
            }
            
            GUI::Text(text);
			PollEvents();
        }
    }

	void CLabel::HandleEvent(CEvent* event)
	{
		Super::HandleEvent(event);
	}
    
} // namespace CE::Widgets


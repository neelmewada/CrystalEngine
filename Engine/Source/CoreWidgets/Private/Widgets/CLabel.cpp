
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
        bool hasPadding = false;
        if (style.styles.KeyExists(CStyleVarType::Padding))
        {
            padding = style.styles[CStyleVarType::Padding].vector;
        }
        
        if (padding.x > 0 || padding.y > 0 || padding.z > 0 || padding.w > 0)
            hasPadding = true;
        
        if (hasPadding)
        {
            Vec2 size = GUI::CalculateTextSize(text);
            Vec2 cursor = GUI::GetCursorPos();
            if (invisibleButtonId.IsEmpty())
            {
                invisibleButtonId = String::Format("CLabel_InvisibleButton##{}", GetUuid());
            }
            GUI::InvisibleButton(invisibleButtonId, size + Vec2(padding.z * 2, padding.w * 2));
            Vec2 finalCursorPos = GUI::GetCursorPos();
            GUI::SetCursorPos(cursor + Vec2(padding.x, padding.y));
            GUI::Text(text);
            GUI::SetCursorPos(finalCursorPos);
        }
        else
        {
            GUI::Text(text);
        }
    }
    
} // namespace CE::Widgets


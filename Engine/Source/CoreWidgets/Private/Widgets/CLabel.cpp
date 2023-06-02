
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
        GUI::Text(text);
    }
    
} // namespace CE::Widgets


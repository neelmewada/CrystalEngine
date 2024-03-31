#pragma once

namespace CE::Widgets
{

    typedef Delegate<bool(const String& text)> CInputValidator;

    CRYSTALWIDGETS_API bool CFloatInputValidator(const String& text);
    
} // namespace CE::Widgets


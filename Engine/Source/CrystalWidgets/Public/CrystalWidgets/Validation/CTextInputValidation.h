#pragma once

namespace CE::Widgets
{

    typedef Delegate<bool(const String& text)> CInputValidator;

    CRYSTALWIDGETS_API bool CFloatInputValidator(const String& text);

    CRYSTALWIDGETS_API bool CIntegerInputValidator(const String& text);

    CRYSTALWIDGETS_API bool CUnsignedIntegerInputValidator(const String& text);

    CRYSTALWIDGETS_API bool CObjectNameInputValidator(const String& text);

    CRYSTALWIDGETS_API bool CBundleNameInputValidator(const String& text);

    CRYSTALWIDGETS_API bool CTypeNameInputValidator(const String& text);
    
} // namespace CE::Widgets


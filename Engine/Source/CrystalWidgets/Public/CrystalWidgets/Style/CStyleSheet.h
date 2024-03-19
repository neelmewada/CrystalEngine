#pragma once

namespace CE::Widgets
{
    CLASS(Abstract)
    class CRYSTALWIDGETS_API CStyleSheet : public Object
    {
        CE_CLASS(CStyleSheet, Object)
    public:

        static CStyleSheet* Load(const IO::Path& path, Object* parent = nullptr);

        void OnSubobjectAttached(Object* subObject) override;
        void OnSubobjectDetached(Object* subobject) override;

        virtual CStyle SelectStyle(CWidget* widget, CStateFlag state = CStateFlag::Default, CSubControl subControl = CSubControl::None) = 0;

        virtual void Clear() = 0;

    crystalwidgets_internal:

        FIELD()
		CStyleSheet* parent = nullptr;


    };
    
} // namespace CE::Widgets

#include "CStyleSheet.rtti.h"
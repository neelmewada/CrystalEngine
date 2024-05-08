#pragma once

namespace CE::Widgets
{
    CLASS(Abstract)
    class CRYSTALWIDGETS_API CStyleSheet : public Object
    {
        CE_CLASS(CStyleSheet, Object)
    public:

        CStyleSheet();
        virtual ~CStyleSheet();

        static CStyleSheet* Load(const IO::Path& path, Object* parent = nullptr);

        void OnSubobjectAttached(Object* subObject) override;
        void OnSubobjectDetached(Object* subobject) override;

        virtual CStyle SelectStyle(CWidget* widget, CStateFlag state = CStateFlag::Default, CSubControl subControl = CSubControl::None) = 0;

        virtual void Clear() = 0;

        static void MarkAllDirty();

        void MarkDirty()
        {
            isDirty = true;
        }

        bool IsDirty() const { return isDirty; }

        FIELD()
		CStyleSheet* parent = nullptr;

        b8 isDirty = true;

    protected:

        static Array<CStyleSheet*> styleSheets;
    };
    
} // namespace CE::Widgets

#include "CStyleSheet.rtti.h"
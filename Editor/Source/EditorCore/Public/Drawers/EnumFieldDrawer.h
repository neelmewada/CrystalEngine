#pragma once

#include "FieldDrawer.h"

namespace CE::Editor
{
    namespace Qt
    {
        class EnumField;
    }

    CLASS()
    class EDITORCORE_API EnumFieldDrawer : public FieldDrawer
    {
        CE_CLASS(EnumFieldDrawer, CE::Editor::FieldDrawer)
    protected:
        EnumFieldDrawer();
    public:
        virtual ~EnumFieldDrawer();

        virtual void OnValuesUpdated() override;

        virtual void CreateGUI(QLayout* container) override;

    private:
        FUNCTION(Event)
        void OnInputValueChanged(s64 newValue);

    private:
        Qt::EnumField* enumField = nullptr;
    };
    
} // namespace CE::Editor

#include "EnumFieldDrawer.rtti.h"

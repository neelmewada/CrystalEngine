#pragma once

#include "FieldDrawer.h"

namespace CE::Editor
{
    namespace Qt
    {
        class EnumField;
    }

    class EDITORCORE_API EnumFieldDrawer : public FieldDrawer
    {
        CE_CLASS(EnumFieldDrawer, FieldDrawer)
    protected:
        EnumFieldDrawer();
    public:
        virtual ~EnumFieldDrawer();

        virtual void OnValuesUpdated() override;

        virtual void CreateGUI(QLayout* container) override;

    private:
        void OnInputValueChanged(s64 newValue);

    private:
        Qt::EnumField* enumField = nullptr;
    };
    
} // namespace CE::Editor

CE_RTTI_CLASS(EDITORCORE_API, CE::Editor, EnumFieldDrawer,
    CE_SUPER(CE::Editor::FieldDrawer),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST(
        CE_FUNCTION(OnInputValueChanged, Event)
    )
)

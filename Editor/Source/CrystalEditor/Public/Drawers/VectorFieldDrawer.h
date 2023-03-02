#pragma once

#include "FieldDrawer.h"

#include "QtComponents/Input/Vec4Input.h"

#include <QObject>

namespace CE::Editor
{
    namespace Qt
    {
        class Vec4Input;
    }

    class CRYSTALEDITOR_API VectorFieldDrawer : public FieldDrawer
    {
        CE_CLASS(VectorFieldDrawer, FieldDrawer)
    protected:
        VectorFieldDrawer();
    public:
        virtual ~VectorFieldDrawer();

        virtual void OnValuesUpdated() override;

        virtual void CreateGUI(QLayout* container) override;

    private:
        void OnValueChanged(Vec4 newValue);

    private:
        Qt::Vec4Input* vec4Field = nullptr;
    };
    
} // namespace CE::Editor

CE_RTTI_CLASS(CRYSTALEDITOR_API, CE::Editor, VectorFieldDrawer,
    CE_SUPER(CE::Editor::FieldDrawer),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST(
        CE_FUNCTION(OnValueChanged, Event)
    )
)

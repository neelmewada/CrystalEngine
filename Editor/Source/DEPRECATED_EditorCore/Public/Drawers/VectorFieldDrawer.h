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

    CLASS()
    class EDITORCORE_API VectorFieldDrawer : public FieldDrawer
    {
        CE_CLASS(VectorFieldDrawer, CE::Editor::FieldDrawer)
    protected:
        VectorFieldDrawer();
    public:
        virtual ~VectorFieldDrawer();

        virtual void OnValuesUpdated() override;

        virtual void CreateGUI(QLayout* container) override;

    private:
        FUNCTION(Event)
        void OnValueChanged(Vec4 newValue);

    private:
        Qt::Vec4Input* vec4Field = nullptr;
    };
    
} // namespace CE::Editor

#include "VectorFieldDrawer.rtti.h"

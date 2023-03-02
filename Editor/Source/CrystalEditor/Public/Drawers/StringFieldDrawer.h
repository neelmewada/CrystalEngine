#pragma once

#include "FieldDrawer.h"

#include "QtComponents/Input/StringField.h"

#include <QObject>

namespace CE::Editor
{
    namespace Qt
    {
        class StringField;
    }

    class CRYSTALEDITOR_API StringFieldDrawer : public FieldDrawer
    {
        CE_CLASS(StringFieldDrawer, FieldDrawer)
    protected:
        StringFieldDrawer();
    public:
        virtual ~StringFieldDrawer();

        virtual void OnValuesUpdated() override;

        virtual void CreateGUI(QLayout* container) override;

    private:
        Qt::StringField* stringField = nullptr;
    };
    
} // namespace CE::Editor

CE_RTTI_CLASS(CRYSTALEDITOR_API, CE::Editor, StringFieldDrawer,
    CE_SUPER(CE::Editor::FieldDrawer),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST()
)

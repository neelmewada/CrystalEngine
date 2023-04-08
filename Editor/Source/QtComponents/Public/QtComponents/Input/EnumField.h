#ifndef ENUMFIELD_H
#define ENUMFIELD_H

#include <QWidget>

#include "CoreMinimal.h"

namespace Ui {
class EnumField;
}

namespace CE::Editor::Qt
{

    class QTCOMPONENTS_API EnumField : public QWidget, public CE::Object
    {
        Q_OBJECT
        CE_CLASS(EnumField, Object)
    public:
        explicit EnumField(QWidget* parent = nullptr);
        virtual ~EnumField();

        void SetEnumType(EnumType* enumType);
        void SetLabel(String label);
        void SetValue(s64 value);

        // Signals

        CE_SIGNAL(OnInputValueChanged, s64);

    private slots:
        void on_comboBox_currentIndexChanged(int index);

    private:
        EnumType* enumType = nullptr;

        Ui::EnumField* ui;
    };

}

CE_RTTI_CLASS(QTCOMPONENTS_API, CE::Editor::Qt, EnumField,
    CE_SUPER(CE::Object),
    CE_DONT_INSTANTIATE,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST(
        // Signals
        CE_FUNCTION(OnInputValueChanged, Signal)
    )
)

#endif // ENUMFIELD_H

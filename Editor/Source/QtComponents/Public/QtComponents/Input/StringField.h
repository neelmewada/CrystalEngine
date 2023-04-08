#ifndef STRINGFIELD_H
#define STRINGFIELD_H

#include <QWidget>

#include "CoreMinimal.h"

namespace Ui {
class StringField;
}

namespace CE::Editor::Qt
{

    class QTCOMPONENTS_API StringField : public QWidget, public CE::Object
    {
        Q_OBJECT
        CE_CLASS(StringField, CE::Object)
    public:
        explicit StringField(QWidget *parent = nullptr);
        virtual ~StringField();

        void SetValue(String value);
        
    signals:
        void OnValueChanged(String newValue);

    private slots:
        void on_textInput_textEdited(const QString &arg1);
        
    public:
        CE_SIGNAL(OnTextInputChanged, CE::String);
        
    private:
        
        Ui::StringField *ui;
    };

}

CE_RTTI_CLASS(QTCOMPONENTS_API, CE::Editor::Qt, StringField,
    CE_SUPER(CE::Object),
    CE_DONT_INSTANTIATE,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST(
        CE_FUNCTION(OnTextInputChanged, Signal)
    )
)

#endif // STRINGFIELD_H

#ifndef STRINGFIELD_H
#define STRINGFIELD_H

#include <QWidget>

namespace Ui {
class StringField;
}

namespace CE::Editor::Qt
{

    class QTCOMPONENTS_API StringField : public QWidget
    {
        Q_OBJECT

    public:
        explicit StringField(QWidget *parent = nullptr);
        ~StringField();

    signals:
        void OnValueChanged(String newValue);

    private slots:
        void on_textInput_textEdited(const QString &arg1);

    private:
        Ui::StringField *ui;
    };

}

#endif // STRINGFIELD_H

#include "StringField.h"
#include "ui_StringField.h"

namespace CE::Editor::Qt
{

    StringField::StringField(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::StringField)
    {
        ui->setupUi(this);
    }

    StringField::~StringField()
    {
        delete ui;
    }

    void StringField::on_textInput_textEdited(const QString &arg1)
    {
        auto text = ui->textInput->text().toStdString();
        String string = String(text.c_str());

        emit OnValueChanged(string);
        OnTextInputChanged(string);
    }

    void StringField::SetValue(String value)
    {
        ui->textInput->setText(QString(value.GetCString()));
    }
}

CE_RTTI_CLASS_IMPL(QTCOMPONENTS_API, CE::Editor::Qt, StringField)

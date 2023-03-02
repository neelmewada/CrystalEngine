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

        emit OnValueChanged(String(text.c_str()));
    }

}

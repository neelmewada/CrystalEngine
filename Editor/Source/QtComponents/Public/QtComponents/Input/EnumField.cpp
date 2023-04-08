#include "EnumField.h"
#include "ui_EnumField.h"

namespace CE::Editor::Qt
{

    EnumField::EnumField(QWidget* parent) :
        QWidget(parent),
        ui(new Ui::EnumField)
    {
        ui->setupUi(this);
    }

    EnumField::~EnumField()
    {
        delete ui;
    }

    void EnumField::SetEnumType(EnumType* enumType)
    {
        if (enumType == nullptr)
            return;

        this->enumType = enumType;

        ui->comboBox->clear();

        for (int i = 0; i < enumType->GetConstantsCount(); i++)
        {
            auto displayName = enumType->GetConstant(i)->GetDisplayName();
            auto value = enumType->GetConstant(i)->GetValue();
            ui->comboBox->addItem(QString(displayName.GetCString()), QVariant(value));
        }
    }

    void EnumField::SetLabel(String label)
    {
        ui->label->setText(label.GetCString());
    }

    void EnumField::SetValue(s64 value)
    {
        int index = ui->comboBox->findData(QVariant(value));
        if (index != -1)
        {
            ui->comboBox->setCurrentIndex(index);
        }
    }


    void EnumField::on_comboBox_currentIndexChanged(int index)
    {
        OnInputValueChanged((s64)ui->comboBox->currentIndex());
    }

}

CE_RTTI_CLASS_IMPL(QTCOMPONENTS_API, CE::Editor::Qt, EnumField)

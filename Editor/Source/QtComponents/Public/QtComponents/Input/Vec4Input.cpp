#include "Vec4Input.h"
#include "ui_Vec4Input.h"

#include <QDoubleValidator>
#include <QIntValidator>

namespace CE::Editor::Qt
{

    Vec4Input::Vec4Input(QWidget* parent) :
        QWidget(parent),
        ui(new Ui::Vec4Input)
    {
        ui->setupUi(this);
    }

    Vec4Input::~Vec4Input()
    {
        delete ui;
    }

    void Vec4Input::SetMode(Vec4Mode mode, bool integerMode)
    {
        this->vecMode = mode;
        this->integerMode = integerMode;

        if (integerMode)
        {
            ui->inputX->setValidator(new QIntValidator(this));
            ui->inputY->setValidator(new QIntValidator(this));
            ui->inputZ->setValidator(new QIntValidator(this));
            ui->inputW->setValidator(new QIntValidator(this));
        }
        else
        {
            constexpr auto min = std::numeric_limits<double>::min();
            constexpr auto max = std::numeric_limits<double>::max();
            int decimals = 4;

            ui->inputX->setValidator(new QDoubleValidator(min, max, decimals, this));
            ui->inputY->setValidator(new QDoubleValidator(min, max, decimals, this));
            ui->inputZ->setValidator(new QDoubleValidator(min, max, decimals, this));
            ui->inputW->setValidator(new QDoubleValidator(min, max, decimals, this));
        }

        ui->inputX->setVisible(true);
        ui->labelX->setVisible(true);
        ui->inputY->setVisible(true);
        ui->labelY->setVisible(true);

        if (mode == Vec4Mode::Vec4)
        {
            ui->inputZ->setVisible(true);
            ui->labelZ->setVisible(true);
            ui->inputW->setVisible(true);
            ui->labelW->setVisible(true);
        }
        else if (mode == Vec4Mode::Vec3)
        {
            ui->inputZ->setVisible(true);
            ui->labelZ->setVisible(true);
            ui->inputW->setVisible(false);
            ui->labelW->setVisible(false);
        }
        else if (mode == Vec4Mode::Vec2)
        {
            ui->inputZ->setVisible(false);
            ui->labelZ->setVisible(false);
            ui->inputW->setVisible(false);
            ui->labelW->setVisible(false);
        }
    }

    void Vec4Input::SetLabel(String label)
    {
        ui->label->setText(QString(label.GetCString()));
    }

    void Vec4Input::SetValue(Vec4 value)
    {
        if (!integerMode)
        {
            ui->inputX->setText(QString::number(value.x));
            ui->inputY->setText(QString::number(value.y));
            ui->inputZ->setText(QString::number(value.z));
            ui->inputW->setText(QString::number(value.w));
        }
        else
        {
            ui->inputX->setText(QString::number((s32)value.x));
            ui->inputY->setText(QString::number((s32)value.y));
            ui->inputZ->setText(QString::number((s32)value.z));
            ui->inputW->setText(QString::number((s32)value.w));
        }
    }

    void Vec4Input::SetValue(Vec4i value)
    {
        if (!integerMode)
        {
            ui->inputX->setText(QString::number(value.x));
            ui->inputY->setText(QString::number(value.y));
            ui->inputZ->setText(QString::number(value.z));
            ui->inputW->setText(QString::number(value.w));
        }
        else
        {
            ui->inputX->setText(QString::number((s32)value.x));
            ui->inputY->setText(QString::number((s32)value.y));
            ui->inputZ->setText(QString::number((s32)value.z));
            ui->inputW->setText(QString::number((s32)value.w));
        }
    }

    void Vec4Input::on_inputX_textChanged(const QString &arg1)
    {
        
    }

    void Vec4Input::on_inputY_textChanged(const QString &arg1)
    {
        
    }

    void Vec4Input::on_inputZ_textChanged(const QString &arg1)
    {
        
    }

    void Vec4Input::on_inputW_textChanged(const QString &arg1)
    {
        
    }

    void Vec4Input::SignalValueChange()
    {
        f32 x = 0, y = 0, z = 0, w = 0;
        ParseFloat(ui->inputX->text(), x);
        ParseFloat(ui->inputY->text(), y);
        ParseFloat(ui->inputZ->text(), z);
        ParseFloat(ui->inputW->text(), w);

        if (listener != nullptr)
            listener->OnValueChanged(Vec4{ x, y, z, w });
        //emit OnValueChanged(x, y, z, w);
    }

    void Vec4Input::ParseFloat(QString string, f32& outFloat)
    {
        bool ok = false;
        f32 value = string.toFloat(&ok);
        if (ok)
        {
            outFloat = value;
        }
    }

    void Vec4Input::on_inputX_textEdited(const QString &arg1)
    {
        SignalValueChange();
    }

    void Vec4Input::on_inputY_textEdited(const QString &arg1)
    {
        SignalValueChange();
    }

    void Vec4Input::on_inputZ_textEdited(const QString &arg1)
    {
        SignalValueChange();
    }

    void Vec4Input::on_inputW_textEdited(const QString &arg1)
    {
        SignalValueChange();
    }

}

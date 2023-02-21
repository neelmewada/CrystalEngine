#include "CardWidget.h"
#include "ui_CardWidget.h"

#include <QPaintEvent>
#include <QPainter>
#include <QToolButton>

namespace CE::Editor::Qt
{

    CardWidget::CardWidget(QWidget* parent) :
        QWidget(parent),
        ui(new Ui::CardWidget)
    {
        ui->setupUi(this);
    }

    CardWidget::~CardWidget()
    {
        delete ui;
    }

    void CardWidget::paintEvent(QPaintEvent* event)
    {
        
    }

    void CardWidget::UpdateFoldout()
    {
        ui->collapseButton->setArrowType(foldToggle ? ::Qt::DownArrow : ::Qt::RightArrow);

        if (foldToggle)
        {
            ui->content->setMaximumHeight(1000000);
        }
        else
        {
            ui->content->setMaximumHeight(0);
        }
    }

    void CardWidget::on_collapseButton_clicked()
    {
        foldToggle = !foldToggle;
        UpdateFoldout();
    }

    void CardWidget::on_headerLabel_clicked()
    {
        foldToggle = !foldToggle;
        UpdateFoldout();
    }

}

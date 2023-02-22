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
        ui->menuButton->setIcon(QIcon(":/icons/vertical-dots"));

        QObject::connect(ui->headerWidget, &QWidget::customContextMenuRequested, this, &CardWidget::handleCustomContextMenu);
        QObject::connect(ui->headerLabel, &QPushButton::customContextMenuRequested, this, &CardWidget::handleCustomContextMenu);
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

    void CardWidget::on_menuButton_clicked()
    {
        emit handleContextMenu(QCursor::pos());
    }

    void CardWidget::handleCustomContextMenu(const QPoint& pos)
    {
        emit handleContextMenu(QCursor::pos());
    }

}


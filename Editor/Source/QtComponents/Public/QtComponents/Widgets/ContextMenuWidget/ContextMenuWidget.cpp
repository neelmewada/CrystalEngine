#include "ContextMenuWidget.h"
#include "ui_ContextMenuWidget.h"

#include <QScreen>

namespace CE::Editor::Qt
{

    ContextMenuWidget::ContextMenuWidget(QWidget *parent, ContextMenuWidget* owner) :
        QWidget(parent),
        owner(owner),
        ui(new Ui::ContextMenuWidget)
    {
        ui->setupUi(this);
        setWindowFlags(::Qt::Popup);
    }

    ContextMenuWidget::~ContextMenuWidget()
    {
        delete ui;
    }

    void ContextMenuWidget::Clear()
    {
        QLayoutItem* item;
        while ((item = ui->containerLayout->takeAt(0)) != NULL)
        {
            delete item->widget();
            delete item;
        }
    }

    ContextMenuCategoryItem* ContextMenuWidget::AddCategoryLabel(QString title)
    {
        ContextMenuCategoryItem* categoryItem = new ContextMenuCategoryItem(this);
        categoryItem->SetTitle(title);

        ui->containerLayout->addWidget(categoryItem);

        return categoryItem;
    }

    ContextMenuRegularItem* ContextMenuWidget::AddRegularItem(QString title, QString icon)
    {
        ContextMenuRegularItem* item = new ContextMenuRegularItem(this);
        item->SetTitle(title);
        if (!icon.isEmpty())
        {
            item->SetIconResource(icon);
        }

        ui->containerLayout->addWidget(item);
        return item;
    }

    void ContextMenuWidget::ShowPopup(QPoint globalPos)
    {
        auto contextMenuSize = this->size();
        auto screenSize = qApp->primaryScreen()->size();

        if (globalPos.x() + contextMenuSize.width() > screenSize.width())
        {
            globalPos.setX(screenSize.width() - contextMenuSize.width() - 10);
        }
        if (globalPos.y() + contextMenuSize.height() > screenSize.height())
        {
            globalPos.setY(screenSize.height() - contextMenuSize.height() - 10);
        }

        this->move(globalPos);
        this->show();
    }

}

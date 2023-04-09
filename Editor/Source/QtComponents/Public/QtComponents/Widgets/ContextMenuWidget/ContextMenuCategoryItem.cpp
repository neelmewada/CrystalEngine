#include "ContextMenuCategoryItem.h"
#include "ui_ContextMenuCategoryItem.h"

namespace CE::Editor::Qt
{

    ContextMenuCategoryItem::ContextMenuCategoryItem(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::ContextMenuCategoryItem)
    {
        ui->setupUi(this);
    }

    ContextMenuCategoryItem::~ContextMenuCategoryItem()
    {
        delete ui;
    }

    void ContextMenuCategoryItem::SetTitle(QString text)
    {
        ui->label->setText(text.toUpper());
    }

}

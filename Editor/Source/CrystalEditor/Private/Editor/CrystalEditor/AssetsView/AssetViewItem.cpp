#include "AssetViewItem.h"
#include "ui_AssetViewItem.h"

#include "System.h"

namespace CE::Editor
{

    AssetViewItem::AssetViewItem(QWidget* parent) :
        QWidget(parent),
        ui(new Ui::AssetViewItem)
    {
        ui->setupUi(this);

        ui->iconLabel->setBackgroundRole(QPalette::Base);
        ui->iconLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        ui->iconLabel->setScaledContents(true);
    }

    AssetViewItem::~AssetViewItem()
    {
        delete ui;
    }

    void AssetViewItem::SetEntry(AssetDatabaseEntry* entry)
    {
        this->entry = entry;

        if (entry == nullptr)
        {
            ui->iconLabel->setStyleSheet("");
            ui->label->setText("");
            return;
        }

        if (entry->entryType == AssetDatabaseEntry::Type::Directory)
        {
            QPixmap image{ ":/Editor/Icons/folder" };
            ui->iconLabel->setPixmap(image);
        }
        else
        {
            QPixmap image{ ":/Editor/Icons/file" };
            ui->iconLabel->setPixmap(image);
        }

        ui->label->setText(QString(entry->name.GetCString()));
    }
}

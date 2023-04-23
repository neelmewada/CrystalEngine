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

        SetRenameMode(false);
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
            ui->renameInput->setText(ui->label->text());
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

        auto text = QString(entry->name.GetCString());
        fullName = text;
        QFontMetrics metrics(ui->label->font());
        QString elidedText = metrics.elidedText(text, ::Qt::ElideRight, ui->label->width());
        ui->label->setText(elidedText);
        ui->renameInput->setText(text);
    }

    void AssetViewItem::SetRenameMode(bool enabled)
    {
        ui->renameInput->setVisible(enabled);
        ui->label->setVisible(!enabled);
        if (enabled)
        {
            ui->renameInput->setFocus();
            ui->renameInput->selectAll();

            if (!focusOutConnected)
            {
                focusOutConnected = true;
                connect(ui->renameInput, &Qt::ExLineEdit::OnFocusOut, this, &AssetViewItem::OnRenameInputFocusOut);
            }
        }
    }

    void AssetViewItem::OnRenameInputFocusOut()
    {
        on_renameInput_editingFinished();
    }

    void AssetViewItem::on_renameInput_editingFinished()
    {
        fullName = ui->renameInput->text();
        emit OnNameInputEdited(ui->renameInput->text());
        SetRenameMode(false);
    }

}

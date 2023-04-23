#ifndef ASSETVIEWITEM_H
#define ASSETVIEWITEM_H

#include <QWidget>

namespace Ui {
class AssetViewItem;
}

namespace CE
{
    class AssetDatabaseEntry;
}

namespace CE::Editor
{

    class CRYSTALEDITOR_API AssetViewItem : public QWidget
    {
        Q_OBJECT
    public:
        explicit AssetViewItem(QWidget* parent = nullptr);
        ~AssetViewItem();

        void SetEntry(AssetDatabaseEntry* entry);

        void SetRenameMode(bool enabled);

    signals:
        void OnNameInputEdited(QString newText);

    private slots:
        void on_renameInput_editingFinished();

        void OnRenameInputFocusOut();

    private:
        AssetDatabaseEntry* entry = nullptr;
        QString fullName = "";
        bool focusOutConnected = false;

        Ui::AssetViewItem* ui;
    };
}

#endif // ASSETVIEWITEM_H

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

    private:
        AssetDatabaseEntry* entry = nullptr;

        Ui::AssetViewItem* ui;
    };
}

#endif // ASSETVIEWITEM_H

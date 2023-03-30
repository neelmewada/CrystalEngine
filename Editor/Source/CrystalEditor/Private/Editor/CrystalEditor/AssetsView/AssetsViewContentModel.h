#ifndef ASSETSVIEWCONTENTMODEL_H
#define ASSETSVIEWCONTENTMODEL_H

#include <QAbstractItemModel>

namespace CE
{
    class AssetDatabaseEntry;
}

namespace CE::Editor
{
    class AssetsViewFolderModel;

    class CRYSTALEDITOR_API AssetsViewContentModel : public QAbstractItemModel
    {
        Q_OBJECT

    public:
        explicit AssetsViewContentModel(QWidget* sizeWidget, QObject *parent = nullptr);

        // Header:
        QVariant headerData(int section, ::Qt::Orientation orientation, int role = ::Qt::DisplayRole) const override;

        // Basic functionality:
        QModelIndex index(int row, int column,
                          const QModelIndex &parent = QModelIndex()) const override;
        QModelIndex parent(const QModelIndex &index) const override;

        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        int columnCount(const QModelIndex &parent = QModelIndex()) const override;

        QVariant data(const QModelIndex &index, int role = ::Qt::DisplayRole) const override;

        void SetDirectoryEntry(AssetDatabaseEntry* entry);

    private:
        QWidget* sizeWidget = nullptr;

        AssetDatabaseEntry* currentDirectory = nullptr;
    };
}

#endif // ASSETSVIEWCONTENTMODEL_H

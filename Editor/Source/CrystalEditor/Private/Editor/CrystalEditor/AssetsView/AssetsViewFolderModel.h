#ifndef ASSETSVIEWFOLDERMODEL_H
#define ASSETSVIEWFOLDERMODEL_H

#include <QAbstractItemModel>

namespace CE
{
    class AssetDatabaseEntry;
}

namespace CE::Editor
{

    class CRYSTALEDITOR_API AssetsViewFolderModel : public QAbstractItemModel
    {
        Q_OBJECT

    public:
        explicit AssetsViewFolderModel(QObject *parent = nullptr);

        // Header:
        QVariant headerData(int section, ::Qt::Orientation orientation, int role = ::Qt::DisplayRole) const override;

        // Basic functionality:
        QModelIndex index(int row, int column,
                          const QModelIndex &parent = QModelIndex()) const override;
        QModelIndex parent(const QModelIndex &index) const override;

        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        int columnCount(const QModelIndex &parent = QModelIndex()) const override;

        QVariant data(const QModelIndex &index, int role = ::Qt::DisplayRole) const override;

        QModelIndex GetIndex(AssetDatabaseEntry* entry);

    private:

    };
}

#endif // ASSETSVIEWFOLDERMODEL_H

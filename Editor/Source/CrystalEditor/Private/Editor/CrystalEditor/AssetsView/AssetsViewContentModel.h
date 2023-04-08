#ifndef ASSETSVIEWCONTENTMODEL_H
#define ASSETSVIEWCONTENTMODEL_H

#include <QAbstractItemModel>
#include <QTableView>

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
        explicit AssetsViewContentModel(QTableView* tableView, QObject *parent = nullptr);

        QModelIndex CreateIndex(int row, int col, void* data);

        // Header:
        QVariant headerData(int section, ::Qt::Orientation orientation, int role = ::Qt::DisplayRole) const override;

        // Basic functionality:
        QModelIndex index(int row, int column,
                          const QModelIndex &parent = QModelIndex()) const override;
        QModelIndex parent(const QModelIndex &index) const override;

        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        int columnCount(const QModelIndex &parent = QModelIndex()) const override;

        QVariant data(const QModelIndex &index, int role = ::Qt::DisplayRole) const override;

        ::Qt::ItemFlags flags(const QModelIndex& index) const override;

        void SetDirectoryEntry(AssetDatabaseEntry* entry);

        // Drag & Drop
        ::Qt::DropActions supportedDropActions() const override;
        QStringList mimeTypes() const override;
        QMimeData* mimeData(const QModelIndexList& indexes) const override;
        bool dropMimeData(const QMimeData* data, ::Qt::DropAction action, int row, int column, const QModelIndex& parent) override;

        static constexpr int sizePerItem = 70;

    private:
        QTableView* tableView = nullptr;

        AssetDatabaseEntry* currentDirectory = nullptr;
    };
}

#endif // ASSETSVIEWCONTENTMODEL_H

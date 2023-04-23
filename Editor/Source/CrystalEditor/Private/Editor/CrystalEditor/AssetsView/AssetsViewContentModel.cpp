#include "AssetsViewContentModel.h"

#include "System.h"

#include "AssetsViewFolderModel.h"

#include <QMimeData>
#include <QMimeDatabase>

namespace CE::Editor
{

    AssetsViewContentModel::AssetsViewContentModel(QTableView* tableView, QObject *parent)
        : QAbstractItemModel(parent)
        , tableView(tableView)
    {

    }

    QModelIndex AssetsViewContentModel::CreateIndex(int row, int col, void* data)
    {
        return createIndex(row, col, data);
    }

    QVariant AssetsViewContentModel::headerData(int section, ::Qt::Orientation orientation, int role) const
    {
        return QVariant();
    }

    QModelIndex AssetsViewContentModel::index(int row, int column, const QModelIndex &parent) const
    {
        if (currentDirectory == nullptr)
            return QModelIndex();

        int maxNumItemsInRow = Math::Max(1, tableView->width() / sizePerItem);

        int totalItemCount = currentDirectory->children.GetSize();
        int numCols = Math::Max(totalItemCount / maxNumItemsInRow, maxNumItemsInRow);

        int index = row * numCols + column;

        if (index >= currentDirectory->children.GetSize())                
            return QModelIndex();
        return createIndex(row, column, currentDirectory->children[index]);
    }

    QModelIndex AssetsViewContentModel::parent(const QModelIndex &index) const
    {
        return QModelIndex();
    }

    int AssetsViewContentModel::rowCount(const QModelIndex &parent) const
    {
        if (parent.isValid() || currentDirectory == nullptr)
            return 0;

        int maxNumItemsInRow = Math::Max(1, tableView->width() / sizePerItem);

        int totalItemCount = currentDirectory->children.GetSize();

        int numCols = Math::Max(totalItemCount / maxNumItemsInRow, maxNumItemsInRow);
        int numRows = totalItemCount / numCols + 1;

        return numRows;
    }

    int AssetsViewContentModel::columnCount(const QModelIndex &parent) const
    {
        if (parent.isValid() || currentDirectory == nullptr)
            return 0;

        int maxNumItemsInRow = Math::Max(1, tableView->width() / sizePerItem);

        int totalItemCount = currentDirectory->children.GetSize();
        
        int numCols = Math::Max(totalItemCount / maxNumItemsInRow, maxNumItemsInRow);

        return numCols;
    }

    QVariant AssetsViewContentModel::data(const QModelIndex &index, int role) const
    {
        if (!index.isValid() || index.internalPointer() == nullptr)
            return QVariant();

        auto item = (AssetDatabaseEntry*)index.internalPointer();
        
        if (role == ::Qt::DisplayRole)
        {
            return QVariant();
        }
        else if (role == ::Qt::DecorationRole)
        {
            return QVariant();
        }
        else if (role == ::Qt::SizeHintRole)
        {
            return QSize(70, 90);
        }

        return QVariant();
    }

    ::Qt::ItemFlags AssetsViewContentModel::flags(const QModelIndex& index) const
    {
        auto defaultFlags = QAbstractItemModel::flags(index);

        if (index.isValid())
            return ::Qt::ItemIsDragEnabled | ::Qt::ItemIsDropEnabled | defaultFlags;
        else
            return ::Qt::ItemIsDropEnabled | defaultFlags;
    }

    void AssetsViewContentModel::SetDirectoryEntry(AssetDatabaseEntry* entry)
    {
        this->currentDirectory = entry;
        emit modelReset({});
    }

    ::Qt::DropActions AssetsViewContentModel::supportedDropActions() const
    {
        return ::Qt::CopyAction | ::Qt::MoveAction | ::Qt::LinkAction | ::Qt::TargetMoveAction;
    }

    QStringList AssetsViewContentModel::mimeTypes() const
    {
        QStringList types;
        // TODO: accepted mime types
        types << "engine/asset-database-entry";
        types << "inode/directory";
        types << "text/plain";
        return types;
    }

    QMimeData* AssetsViewContentModel::mimeData(const QModelIndexList& indexes) const
    {
        // TODO: currently dragged mime type
        auto mimeData = new QMimeData();
        QByteArray encodedData;

        QDataStream stream(&encodedData, QIODevice::WriteOnly);
        
        foreach(const QModelIndex & index, indexes) 
        {
            if (index.isValid() && index.internalPointer() != nullptr) 
            {
                auto entry = (AssetDatabaseEntry*)index.internalPointer();
                
                QString text = data(index, ::Qt::DisplayRole).toString();
                stream << (qsizetype)entry;
                stream << text;
            }
        }

        mimeData->setData("engine/asset-database-entry", encodedData);
        return mimeData;
    }

    bool AssetsViewContentModel::dropMimeData(const QMimeData* data, ::Qt::DropAction action, int row, int column, const QModelIndex& parent)
    {
        Array<IO::Path> localFilePaths{};
        for (auto url : data->urls())
        {
            if (!url.isLocalFile())
                continue;
            IO::Path path = url.path().toStdString().substr(1);
            if (path.Exists())
                localFilePaths.Add(path);
        }

        if (localFilePaths.GetSize() > 0)
        {
            if (currentDirectory == nullptr)
                return false;
            return AssetManager::Get().ImportExternalAssets(localFilePaths, currentDirectory);
        }

        for (const auto& formatStr : data->formats())
        {
            // TODO: Internal content dropped
            CE_LOG(Info, All, "Drop Format: {}", String(formatStr.toStdString()));
        }

        return false;
    }

    bool AssetsViewContentModel::canDropMimeData(const QMimeData* data, ::Qt::DropAction action, int row, int column, const QModelIndex& parent) const
    {
        Array<IO::Path> localFilePaths{};
        for (auto url : data->urls())
        {
            if (!url.isLocalFile())
                continue;
            IO::Path path = url.path().toStdString().substr(1);
            if (path.Exists())
				localFilePaths.Add(path);
        }

        if (localFilePaths.GetSize() > 0)
        {
            // External content dropped
            return true;
        }

        if (!data->formats().empty())
        {
            // Internal content dropped
            return true;
        }

        return false;
    }
}

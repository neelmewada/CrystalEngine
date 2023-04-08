#include "AssetsViewContentModel.h"

#include "System.h"

#include "AssetsViewFolderModel.h"

#include <QMimeData>

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
        return createIndex(row, column, &currentDirectory->children[index]);
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
            //return QString(item->name.GetCString());
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
        return ::Qt::CopyAction | ::Qt::MoveAction | ::Qt::LinkAction;
    }

    QStringList AssetsViewContentModel::mimeTypes() const
    {
        QStringList types;
        types << "ce-mime-format";
        return types;
    }

    QMimeData* AssetsViewContentModel::mimeData(const QModelIndexList& indexes) const
    {
        QMimeData* mimeData = new QMimeData();
        QByteArray encodedData;

        QDataStream stream(&encodedData, QIODevice::WriteOnly);
        
        foreach(const QModelIndex & index, indexes) 
        {
            if (index.isValid()) 
            {
                QString text = data(index, ::Qt::DisplayRole).toString();
                stream << text;
            }
        }

        mimeData->setData("ce-mime-format", encodedData);
        return mimeData;
    }

    bool AssetsViewContentModel::dropMimeData(const QMimeData* data, ::Qt::DropAction action, int row, int column, const QModelIndex& parent)
    {
        return false;
    }
}

#include "AssetsViewContentModel.h"

#include "System.h"

#include "AssetsViewFolderModel.h"

namespace CE::Editor
{
    constexpr int sizePerItem = 512;

    AssetsViewContentModel::AssetsViewContentModel(QWidget* sizeWidget, QObject *parent)
        : QAbstractItemModel(parent)
        , sizeWidget(sizeWidget)
    {

    }

    QVariant AssetsViewContentModel::headerData(int section, ::Qt::Orientation orientation, int role) const
    {
        return QVariant();
    }

    QModelIndex AssetsViewContentModel::index(int row, int column, const QModelIndex &parent) const
    {
        if (currentDirectory == nullptr)
            return QModelIndex();

        auto width = sizeWidget->width();
        int maxNumItemsInRow = Math::Max(1, width / sizePerItem);

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

        auto width = sizeWidget->width();
        int maxNumItemsInRow = Math::Max(1, width / sizePerItem);

        int totalItemCount = currentDirectory->children.GetSize();

        int numCols = Math::Max(totalItemCount / maxNumItemsInRow, maxNumItemsInRow);
        int numRows = totalItemCount / numCols + totalItemCount % numCols;

        return numRows;
    }

    int AssetsViewContentModel::columnCount(const QModelIndex &parent) const
    {
        if (parent.isValid() || currentDirectory == nullptr)
            return 0;

        auto width = sizeWidget->width();
        int maxNumItemsInRow = Math::Max(1, width / sizePerItem);

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
            return QString(item->name.GetCString());
        }
        else if (role == ::Qt::DecorationRole)
        {
            if (item->entryType == AssetDatabaseEntry::Type::Asset)
                return QIcon(":/Editor/Icons/file");
            return QIcon(":/Editor/Icons/folder");
        }

        return QVariant();
    }

    void AssetsViewContentModel::SetDirectoryEntry(AssetDatabaseEntry* entry)
    {
        this->currentDirectory = entry;
        emit modelReset({});
    }
}

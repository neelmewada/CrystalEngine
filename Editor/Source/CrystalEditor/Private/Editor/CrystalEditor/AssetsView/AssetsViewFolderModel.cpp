#include "AssetsViewFolderModel.h"

#include "System.h"

namespace CE::Editor
{

    AssetsViewFolderModel::AssetsViewFolderModel(QObject *parent)
        : QAbstractItemModel(parent)
    {
        
    }

    QVariant AssetsViewFolderModel::headerData(int section, ::Qt::Orientation orientation, int role) const
    {
        return QVariant();
    }

    QModelIndex AssetsViewFolderModel::index(int row, int column, const QModelIndex &parent) const
    {
        auto root = AssetDatabase::Get().GetRootEntry();
        if (!parent.isValid())
            return createIndex(row, column, root->children[row]);

        auto entry = (AssetDatabaseEntry*)parent.internalPointer();
        if (entry == nullptr)
            return QModelIndex();

        int idx = 0;
        for (int i = 0; i < entry->children.GetSize(); i++)
        {
            if (entry->children[i]->entryType == AssetDatabaseEntry::Type::Directory)
            {
                if (idx == row)
                {
                    return createIndex(row, column, entry->children[row]);
                }
                idx++;
            }
        }

        return QModelIndex();
    }

    QModelIndex AssetsViewFolderModel::parent(const QModelIndex &index) const
    {
        auto root = AssetDatabase::Get().GetRootEntry();

        AssetDatabaseEntry* entry = (AssetDatabaseEntry*)index.internalPointer();
        if (entry == nullptr)
            return QModelIndex();

        AssetDatabaseEntry* parentEntry = entry->parent;
        if (parentEntry == nullptr || parentEntry == root)
            return QModelIndex();

        const AssetDatabaseEntry* parentsParent = parentEntry->parent;
        if (parentsParent == nullptr)
        {
            parentsParent = AssetDatabase::Get().GetRootEntry();
        }

        int idx = 0;
        for (int i = 0; i < parentsParent->children.GetSize(); i++)
        {
            if (parentEntry == parentsParent->children[i])
            {
                return createIndex(idx, 0, parentEntry);
            }
            if (parentEntry->children[i]->entryType == AssetDatabaseEntry::Type::Directory)
            {
                idx++;
            }
        }

        return QModelIndex();
    }

    int AssetsViewFolderModel::rowCount(const QModelIndex &parent) const
    {
        auto root = AssetDatabase::Get().GetRootEntry();

        if (!parent.isValid())
            return root->children.GetSize();

        auto entry = (AssetDatabaseEntry*)parent.internalPointer();
        if (entry == nullptr)
            return 0;

        int count = 0;
        for (int i = 0; i < entry->children.GetSize(); i++)
        {
            if (entry->children[i]->entryType == AssetDatabaseEntry::Type::Directory)
                count++;
        }

        return count;
    }

    int AssetsViewFolderModel::columnCount(const QModelIndex &parent) const
    {
        return 1;
    }

    QVariant AssetsViewFolderModel::data(const QModelIndex &index, int role) const
    {
        if (!index.isValid() || index.internalPointer() == nullptr)
            return QVariant();

        AssetDatabaseEntry* entry = (AssetDatabaseEntry*)index.internalPointer();
        
        if (role == ::Qt::DecorationRole)
        {
            return QIcon(":/Editor/Icons/folder");
        }

        if (role != ::Qt::DisplayRole)
            return QVariant();

        return QVariant(entry->name.GetCString());
    }

    QModelIndex AssetsViewFolderModel::GetIndex(AssetDatabaseEntry* entry)
    {
        auto root = AssetDatabase::Get().GetRootEntry();

        if (entry == nullptr || entry->parent == nullptr || entry == root)
            return QModelIndex();

        for (int i = 0; i < entry->parent->children.GetSize(); i++)
        {
            if (entry->parent->children[i] == entry)
            {
                return index(i, 0, GetIndex(entry->parent));
            }
        }

        return QModelIndex();
    }

}

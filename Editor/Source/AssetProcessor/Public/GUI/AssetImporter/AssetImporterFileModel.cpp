#include "AssetImporterFileModel.h"

namespace CE::Editor
{

    AssetImporterFileModel::AssetImporterFileModel(IO::Path path, QObject *parent)
        : QAbstractItemModel(parent)
        , path(path)
    {
        root = new AssetImporterFileModelEntry;
    }

    AssetImporterFileModel::~AssetImporterFileModel()
    {
        delete root;
        root = nullptr;
    }

    void AssetImporterFileModel::UpdateDirectoryData()
    {
        if (!path.Exists())
            return;

        path.RecursivelyIterateChildren([this](const IO::Path& path) -> void
        {
            if (!path.Exists())
                return;
            
            if (path.IsDirectory())
                return;

            auto relPath = IO::Path::GetRelative(path, this->path);

            AssetImporterFileModelEntry* entry = root;
            IO::Path curPath = "";

            for (auto it = relPath.begin(); it != relPath.end(); ++it)
            {
                auto str = String((*it).string());
                curPath = curPath / str;

                if (!entry->HasChild(str)) // Create child entry
                {
                    auto thisEntry = new AssetImporterFileModelEntry(entry);
                    entry->children.Add(thisEntry);
                    entry = thisEntry;

                    entry->name = str;
                    entry->fullPath = curPath;
                }
                else // Use existing child entry
                {
                    entry = entry->GetChild(str);
                }
            }

            if (entry->IsTerminal() && entry != root)
                allFileEntries.Add(entry);
        });

        emit modelReset({});
    }

    void AssetImporterFileModel::SetDirectory(IO::Path path)
    {
        this->path = path;
    }

    void AssetImporterFileModel::SetTreeView(bool set)
    {
        showTreeView = set;
        emit modelReset({});
    }

    QVariant AssetImporterFileModel::headerData(int section, ::Qt::Orientation orientation, int role) const
    {
        return QVariant();
    }

    QModelIndex AssetImporterFileModel::index(int row, int column, const QModelIndex &parent) const
    {
        if (!showTreeView)
        {
            return createIndex(row, column, allFileEntries[row]);
        }

        if (!parent.isValid())
        {
            auto entry = root->children[row];
            return createIndex(row, column, entry);
        }

        auto parentEntry = (AssetImporterFileModelEntry*)parent.internalPointer();
        if (parentEntry == nullptr)
            return QModelIndex();

        return createIndex(row, column, parentEntry->children[row]);
    }

    QModelIndex AssetImporterFileModel::parent(const QModelIndex &index) const
    {
        if (!showTreeView)
        {
            return QModelIndex();
        }

        auto entry = (AssetImporterFileModelEntry*)index.internalPointer();

        if (entry == nullptr || entry->parent == nullptr)
            return QModelIndex();

        auto parentParentEntry = entry->parent->parent;

        if (parentParentEntry == nullptr)
            parentParentEntry = root;

        int idx = parentParentEntry->children.IndexOf(entry->parent);
        return createIndex(idx, 0, entry->parent);
    }

    int AssetImporterFileModel::rowCount(const QModelIndex &parent) const
    {
        if (!showTreeView)
        {
            if (parent.isValid())
                return 0;
            return allFileEntries.GetSize();
        }

        if (!parent.isValid()) // root entries
        {
            return root->children.GetSize();
        }

        auto parentEntry = (AssetImporterFileModelEntry*)parent.internalPointer();
        if (parentEntry == nullptr)
            return 0;

        return parentEntry->children.GetSize();
    }

    int AssetImporterFileModel::columnCount(const QModelIndex &parent) const
    {
        return 1;
    }

    QVariant AssetImporterFileModel::data(const QModelIndex &index, int role) const
    {
        if (!index.isValid())
            return QVariant();

        auto entry = (AssetImporterFileModelEntry*)index.internalPointer();
        if (entry == nullptr)
            return QVariant();

        if (role == ::Qt::DecorationRole)
        {
            if (entry->IsTerminal())
                return QIcon(":/Editor/Icons/file");
            return QIcon(":/Editor/Icons/folder");
        }

        if (role != ::Qt::DisplayRole)
        {
            return QVariant();
        }
        
        String name = entry->name.GetString();
        return QString(name.GetCString());
    }
}

#include "AssetImporterFileFilterModel.h"

#include "AssetImporterFileModel.h"

namespace CE::Editor
{

    AssetImporterFileFilterModel::AssetImporterFileFilterModel(QObject* parent)
        : QSortFilterProxyModel(parent)
    {

    }

    AssetImporterFileFilterModel::~AssetImporterFileFilterModel()
    {

    }

    bool AssetImporterFileFilterModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
    {
        auto parentEntry = (AssetImporterFileModelEntry*)source_parent.internalPointer();

        auto source = (AssetImporterFileModel*)sourceModel();

        if (source == nullptr)
            return true;

        auto children = &source->allFileEntries;
        
        if (source->showTreeView)
        {
            if (parentEntry == nullptr)
                parentEntry = source->root;
            children = &parentEntry->children;
        }

        if (source_row >= children->GetSize())
            return true;

        auto entry = children->At(source_row);
        
        if (entry->fullPath.IsDirectory())
        {
            return true;
        }

        if (showProcessedFiles && entry->IsProcessed())
        {
            return true;
        }

        if (showUnprocessedFiles && !entry->IsProcessed())
        {
            return true;
        }

        return false;
    }

    bool AssetImporterFileFilterModel::filterAcceptsColumn(int source_column, const QModelIndex& source_parent) const
    {
        return true;
    }
}

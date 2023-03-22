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

        const auto& children = source->allFileEntries;

        if (parentEntry == nullptr && source->showTreeView)
        {
            parentEntry = source->root;
        }

        if (source_row >= children.GetSize())
            return true;

        auto entry = children[source_row];

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

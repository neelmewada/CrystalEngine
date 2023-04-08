#ifndef ASSETIMPORTERFILEFILTERMODEL_H
#define ASSETIMPORTERFILEFILTERMODEL_H

#include <QSortFilterProxyModel>
#include <QObject>

namespace CE::Editor
{

    class AssetImporterFileFilterModel : public QSortFilterProxyModel
    {
        Q_OBJECT
    public:
        AssetImporterFileFilterModel(QObject* parent = nullptr);
        virtual ~AssetImporterFileFilterModel();

        virtual bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;
        virtual bool filterAcceptsColumn(int source_column, const QModelIndex& source_parent) const override;

    private:
        bool showProcessedFiles = true;
        bool showUnprocessedFiles = true;

        friend class AssetImporterWindow;
    };

}

#endif // ASSETIMPORTERFILEFILTERMODEL_H

#include "AssetsViewContentModel.h"

#include "System.h"

namespace CE::Editor
{
    AssetsViewContentModel::AssetsViewContentModel(QObject *parent)
        : QAbstractItemModel(parent)
    {

    }

    QVariant AssetsViewContentModel::headerData(int section, ::Qt::Orientation orientation, int role) const
    {
        return QVariant();
    }

    QModelIndex AssetsViewContentModel::index(int row, int column, const QModelIndex &parent) const
    {
        return QModelIndex();
    }

    QModelIndex AssetsViewContentModel::parent(const QModelIndex &index) const
    {
        return QModelIndex();
    }

    int AssetsViewContentModel::rowCount(const QModelIndex &parent) const
    {
        if (!parent.isValid())
            return 0;

        return 0;
    }

    int AssetsViewContentModel::columnCount(const QModelIndex &parent) const
    {
        if (!parent.isValid())
            return 0;

        return 0;
    }

    QVariant AssetsViewContentModel::data(const QModelIndex &index, int role) const
    {
        if (!index.isValid())
            return QVariant();

        // FIXME: Implement me!
        return QVariant();
    }
}

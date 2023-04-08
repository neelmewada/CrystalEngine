#include "ConsoleViewModel.h"

namespace CE::Editor
{

    ConsoleViewModel::ConsoleViewModel(QObject *parent)
        : QAbstractItemModel(parent)
    {
    }

    ConsoleViewModel::~ConsoleViewModel() noexcept
    {

    }

    QVariant ConsoleViewModel::headerData(int section, ::Qt::Orientation orientation, int role) const
    {
        return QVariant();
    }

    QModelIndex ConsoleViewModel::index(int row, int column, const QModelIndex &parent) const
    {
        if (parent.isValid())
            return QModelIndex();

        return createIndex(row, column, nullptr);
    }

    QModelIndex ConsoleViewModel::parent(const QModelIndex &index) const
    {
        return QModelIndex();
    }

    int ConsoleViewModel::rowCount(const QModelIndex &parent) const
    {
        if (parent.isValid())
            return 0;

        return GetEntries().GetSize();
    }

    int ConsoleViewModel::columnCount(const QModelIndex &parent) const
    {
        if (parent.isValid())
            return 0;

        return 1;
    }

    bool ConsoleViewModel::hasChildren(const QModelIndex &parent) const
    {
        return false;
    }

    bool ConsoleViewModel::canFetchMore(const QModelIndex &parent) const
    {
        return false;
    }

    void ConsoleViewModel::fetchMore(const QModelIndex &parent)
    {
        
    }

    QVariant ConsoleViewModel::data(const QModelIndex &index, int role) const
    {
        if (!index.isValid())
            return QVariant();
        if (role == ::Qt::SizeHintRole)
            return QSize(1000, 25);
        if (role != ::Qt::DisplayRole)
            return QVariant();

        const String& str = GetEntries()[index.row()];
        return QString(str.GetCString());
    }

    String ConsoleViewModel::GetEntryDetailsFor(const QModelIndex& entryIndex)
    {
        if (!entryIndex.isValid())
            return "";
        if (entryIndex.row() >= GetEntries().GetSize())
            return "";
        

        return GetEntries()[entryIndex.row()];
    }

    void ConsoleViewModel::SetSearchFilter(const String& filter)
    {
        filteredEntries.Clear();

        for (const auto& entry : entries)
        {
            if (entry.Search(filter))
            {
                filteredEntries.Add(entry);
            }
        }

        emit modelReset({});
        emit ShouldRefreshSelection();
    }

    void ConsoleViewModel::ClearLogs()
    {
        filteredEntries.Clear();
        entries.Clear();

        emit modelReset({});
        emit ShouldRefreshSelection();
    }

    const CE::Array<CE::String>& ConsoleViewModel::GetEntries() const
    {
        if (filteredEntries.GetSize() > 0)
            return filteredEntries;
        return entries;
    }

}

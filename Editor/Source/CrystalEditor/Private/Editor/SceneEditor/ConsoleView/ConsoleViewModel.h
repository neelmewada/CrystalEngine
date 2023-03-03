#ifndef CONSOLEVIEWMODEL_H
#define CONSOLEVIEWMODEL_H

#include <QAbstractItemModel>


namespace CE::Editor
{

    class ConsoleViewModel : public QAbstractItemModel
    {
        Q_OBJECT

    public:
        explicit ConsoleViewModel(QObject *parent = nullptr);
        virtual ~ConsoleViewModel() noexcept;

        // Header:
        QVariant headerData(int section, ::Qt::Orientation orientation, int role = ::Qt::DisplayRole) const override;

        // Basic functionality:
        QModelIndex index(int row, int column,
                          const QModelIndex &parent = QModelIndex()) const override;
        QModelIndex parent(const QModelIndex &index) const override;

        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        int columnCount(const QModelIndex &parent = QModelIndex()) const override;

        // Fetch data dynamically:
        bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;

        bool canFetchMore(const QModelIndex &parent) const override;
        void fetchMore(const QModelIndex &parent) override;

        QVariant data(const QModelIndex &index, int role = ::Qt::DisplayRole) const override;

        void PushEntry(const CE::String& entry)
        {
            entries.Add(entry);
            emit modelReset({});
        }

        String GetEntryDetailsFor(const QModelIndex& entryIndex);

        void SetSearchFilter(const String& filter);

        void ClearLogs();

    signals:
        void ShouldRefreshSelection();

    private:

        const CE::Array<CE::String>& GetEntries() const;

        CE::Array<CE::String> filteredEntries{};
        CE::Array<CE::String> entries{};
    };

}

#endif // CONSOLEVIEWMODEL_H

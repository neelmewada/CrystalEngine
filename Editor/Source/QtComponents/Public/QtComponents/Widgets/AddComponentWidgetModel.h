#ifndef ADDCOMPONENTWIDGETMODEL_H
#define ADDCOMPONENTWIDGETMODEL_H

#include <QAbstractItemModel>
#include <QObject>

namespace CE::Editor::Qt
{
    struct ComponentEntry
    {
        String name = "";
        Array<ComponentEntry> children{};

        bool isTerminal = false;
        CE::Name fullName{};
        ComponentEntry* parent = nullptr;
        int siblingIndex = 0;
    };
    
    class QTCOMPONENTS_API AddComponentWidgetModel : public QAbstractItemModel
    {
        Q_OBJECT
    public:
        AddComponentWidgetModel(QObject* parent = nullptr);
        virtual ~AddComponentWidgetModel();

        //::Qt::ItemFlags flags(const QModelIndex &index) const override;
        QVariant data(const QModelIndex& index, int role) const override;
        QVariant headerData(int section, ::Qt::Orientation orientation, int role = ::Qt::DisplayRole) const override;

        QModelIndex index(int row, int column,
            const QModelIndex& parent = QModelIndex()) const override;
        QModelIndex parent(const QModelIndex& index) const override;
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        int columnCount(const QModelIndex& parent = QModelIndex()) const override;

        void SetSearchText(QString text);

        inline bool IsSearchMode() const
        {
            return !searchText.isEmpty();
        }

        ClassType* GetComponentTypeFromIndex(const QModelIndex& index, Name& outName);

    public slots:
        void UpdateComponentRegistry();

    private:
        int CalculateTerminalEntriesCount(ComponentEntry* entry) const;

        void PopulateSearchEntries();

        void PopulateSearchEntries(ComponentEntry* entry);

        ComponentEntry* rootEntry = nullptr;
        Array<ComponentEntry*> searchEntries{};
        QString searchText = "";
    };

}

#endif // ADDCOMPONENTWIDGETMODEL_H

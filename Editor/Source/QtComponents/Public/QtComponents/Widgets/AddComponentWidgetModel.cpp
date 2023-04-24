#include "AddComponentWidgetModel.h"

#include "GameFramework/GameFramework.h"

namespace CE::Editor::Qt
{
    
    AddComponentWidgetModel::AddComponentWidgetModel(QObject* parent)
        : QAbstractItemModel(parent)
    {
        GameComponentRegistry::Get().SortEntries();

        rootEntry = new ComponentEntry;

        UpdateComponentRegistry();
    }

    AddComponentWidgetModel::~AddComponentWidgetModel()
    {

    }

    QVariant AddComponentWidgetModel::data(const QModelIndex& index, int role) const
    {
        if (role != ::Qt::DisplayRole)
            return QVariant();

        auto entry = (ComponentEntry*)index.internalPointer();
        if (entry == nullptr)
            return QVariant();

        auto str = !IsSearchMode() ? entry->name : entry->fullName.GetString();
        return QString(str.GetCString());
    }

    QVariant AddComponentWidgetModel::headerData(int section, ::Qt::Orientation orientation, int role) const
    {
        return QVariant();
    }

    QModelIndex AddComponentWidgetModel::index(int row, int column, const QModelIndex& parent) const
    {
        if (parent.isValid())
        {
            if (IsSearchMode())
                return QModelIndex();

            if (parent.internalPointer() == nullptr)
                return QModelIndex();
            auto parentEntry = (ComponentEntry*)parent.internalPointer();
            return createIndex(row, column, &parentEntry->children[row]);
        }

        if (IsSearchMode())
        {
            return createIndex(row, column, searchEntries[row]);
        }
        
        return createIndex(row, column, &rootEntry->children[row]);
    }

    QModelIndex AddComponentWidgetModel::parent(const QModelIndex& index) const
    {
        if (!index.isValid() || index.internalPointer() == nullptr || IsSearchMode())
            return QModelIndex();

        auto entry = (ComponentEntry*)index.internalPointer();
        if (entry->parent == nullptr)
            return QModelIndex();

        return createIndex(entry->siblingIndex, 0, entry->parent);
    }

    int AddComponentWidgetModel::rowCount(const QModelIndex& parent) const
    {
        if (parent.isValid())
        {
            if (IsSearchMode())
                return 0;

            if (parent.internalPointer() == nullptr)
                return 0;
            ComponentEntry* entry = (ComponentEntry*)parent.internalPointer();
            return entry->children.GetSize();
        }

        if (IsSearchMode())
        {
            return searchEntries.GetSize();
        }

        return rootEntry->children.GetSize();
    }

    int AddComponentWidgetModel::columnCount(const QModelIndex& parent) const
    {
        return 1;
    }

    void AddComponentWidgetModel::SetSearchText(QString text)
    {
        this->searchText = text;
        PopulateSearchEntries();
    }

    void AddComponentWidgetModel::UpdateComponentRegistry()
    {
        // Clear all entries
        rootEntry->children.Clear();

        Array<ClassType*> gameComponentSubclasses = GameComponent::Type()->GetDerivedClasses();

        for (int i = 0; i < gameComponentSubclasses.GetSize(); i++)
        {
            auto componentEntry = gameComponentSubclasses[i];

            if (componentEntry == nullptr || !componentEntry->CanBeInstantiated())
                continue;

            auto name = componentEntry->GetName();

            CE::Array<String> nameComponents{};
            name.GetComponents(nameComponents);

            auto currentEntry = rootEntry;
            String nameBuilder = "";

            for (int j = 0; j < nameComponents.GetSize(); j++)
            {
                const CE::String& nameComponent = nameComponents[j];
                int idx = currentEntry->children.IndexOf([&](const ComponentEntry& entry) -> bool
                    {
                        return entry.name == nameComponent;
                    });

                if (idx >= 0) // Exists
                {
                    currentEntry = &currentEntry->children[idx];
                    continue;
                }
                else
                {
                    currentEntry->children.Add(ComponentEntry{ nameComponent, Array<ComponentEntry>{} });

                    auto newEntry = &currentEntry->children.GetLast();
                    newEntry->siblingIndex = currentEntry->children.GetSize() - 1;
                    newEntry->parent = currentEntry;

                    currentEntry = newEntry;

                    if (!nameBuilder.IsEmpty())
                        nameBuilder += "::";
                    nameBuilder += nameComponent;

                    if (j == nameComponents.GetSize() - 1) // Last name component
                    {
                        currentEntry->isTerminal = true;
                        currentEntry->fullName = name;
                        break;
                    }
                    else
                    {
                        currentEntry->fullName = nameBuilder;
                    }
                }
            }
        }

        emit modelReset({});
    }

    ClassType* AddComponentWidgetModel::GetComponentTypeFromIndex(const QModelIndex& index, Name& outName)
    {
        outName = CE::Name();
        if (!index.isValid() || index.internalPointer() == nullptr)
            return nullptr;

        ComponentEntry* entry = (ComponentEntry*)index.internalPointer();

        outName = entry->fullName;

        if (!entry->isTerminal || !entry->fullName.IsValid())
            return nullptr;

        ClassType* type = (ClassType*)GetTypeInfo(entry->fullName);
        return type;
    }

    int AddComponentWidgetModel::CalculateTerminalEntriesCount(ComponentEntry* entry) const
    {
        int count = 0;

        if (entry->children.GetSize() == 0)
        {
            return 1;
        }

        for (int i = 0; i < entry->children.GetSize(); i++)
        {
            count += CalculateTerminalEntriesCount(&entry->children[i]);
        }

        return count;
    }

    void AddComponentWidgetModel::PopulateSearchEntries()
    {
        auto currentEntry = rootEntry;
        searchEntries.Clear();

        for (int i = 0; i < currentEntry->children.GetSize(); i++)
        {
            PopulateSearchEntries(&currentEntry->children[i]);
        }

        emit modelReset({});
    }

    void AddComponentWidgetModel::PopulateSearchEntries(ComponentEntry* entry)
    {
        if (entry == nullptr)
            return;

        std::string searchString = searchText.toStdString();

        if (entry->children.GetSize() == 0)
        {
            String nameString = entry->fullName.GetString();
            if (nameString.Search(searchString.c_str()))
            {
                searchEntries.Add(entry);
            }
            return;
        }

        for (int i = 0; i < entry->children.GetSize(); i++)
        {
            PopulateSearchEntries(&entry->children[i]);
        }
    }

}

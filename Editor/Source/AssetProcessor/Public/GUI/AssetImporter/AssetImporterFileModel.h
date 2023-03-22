#ifndef ASSETPROCESSORFILEMODEL_H
#define ASSETPROCESSORFILEMODEL_H

#include <QAbstractItemModel>

#include "CoreMinimal.h"

namespace CE::Editor
{
    struct AssetImporterFileModelEntry
    {
        AssetImporterFileModelEntry(AssetImporterFileModelEntry* parent = nullptr)
            : parent(parent)
        {

        }

        virtual ~AssetImporterFileModelEntry()
        {
            for (auto child : children)
            {
                delete child;
            }
            children.Clear();
        }

        bool IsTerminal()
        {
            return children.GetSize() == 0;
        }

        bool HasChild(Name name)
        {
            for (auto child : children)
            {
                if (child->name == name)
                    return true;
            }
            return false;
        }

        AssetImporterFileModelEntry* GetChild(Name name)
        {
            for (auto child : children)
            {
                if (child->name == name)
                    return child;
            }
            return nullptr;
        }

        bool IsProcessed()
        {
            if (fullPath.IsDirectory())
                return false;
            auto assetFile = fullPath.GetParentPath() / (fullPath.GetFilename().RemoveExtension().GetString() + ".casset");
            return assetFile.Exists();
        }

        String GetExtension()
        {
            return fullPath.GetExtension().GetString();
        }

    public:
        AssetImporterFileModelEntry* parent = nullptr;

        Name name{};
        IO::Path fullPath{};
        IO::Path relPath{};
        Array<AssetImporterFileModelEntry*> children{};
    };

    class AssetImporterFileModel : public QAbstractItemModel
    {
        Q_OBJECT
    public:
        explicit AssetImporterFileModel(IO::Path path, QObject *parent = nullptr);
        virtual ~AssetImporterFileModel();

        void UpdateDirectoryData();

        void SetDirectory(IO::Path path);

        void SetTreeView(bool set);

        // Header:
        QVariant headerData(int section, ::Qt::Orientation orientation, int role = ::Qt::DisplayRole) const override;

        // Basic functionality:
        QModelIndex index(int row, int column,
                          const QModelIndex &parent = QModelIndex()) const override;
        QModelIndex parent(const QModelIndex &index) const override;

        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        int columnCount(const QModelIndex &parent = QModelIndex()) const override;

        QVariant data(const QModelIndex &index, int role = ::Qt::DisplayRole) const override;

    private:
        Array<AssetImporterFileModelEntry*> allFileEntries{};

        AssetImporterFileModelEntry* root = nullptr;
        bool showTreeView = true;
        bool hideProcessedFiles = true;

        IO::Path path{};

        friend class AssetImporterWindow;
        friend class AssetImporterFileFilterModel;
    };
}

#endif // ASSETPROCESSORFILEMODEL_H

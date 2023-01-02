#pragma once

#include <QAbstractItemModel>
#include <QVariant>
#include <QModelIndex>

namespace CE
{
    class Scene;
}

namespace CE::Editor
{

    class CRYSTALEDITOR_API SceneOutlinerModel : public QAbstractItemModel
    {
        Q_OBJECT
        
    public:
        explicit SceneOutlinerModel(QObject* parent = nullptr);
        virtual ~SceneOutlinerModel();
        
        void OnSceneOpened(Scene* scene);
        void OnSceneClosed(Scene* scene);
        
        CE_INLINE Scene* GetScene() const
        {
            return currentScene;
        }
        
        QVariant data(const QModelIndex &index, int role) const override;
        ::Qt::ItemFlags flags(const QModelIndex &index) const override;
        QVariant headerData(int section, ::Qt::Orientation orientation, int role = ::Qt::DisplayRole) const override;
        
        QModelIndex index(int row, int column,
                          const QModelIndex &parent = QModelIndex()) const override;
        QModelIndex parent(const QModelIndex &index) const override;
        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        int columnCount(const QModelIndex &parent = QModelIndex()) const override;
        
    private:
        Scene* currentScene = nullptr;
    };
    
} // namespace CE::Editor

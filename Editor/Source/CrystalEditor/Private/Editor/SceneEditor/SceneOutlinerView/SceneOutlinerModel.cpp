
#include "SceneOutlinerModel.h"

#include "CoreMinimal.h"
#include "GameFramework/GameFramework.h"

namespace CE::Editor
{

    SceneOutlinerModel::SceneOutlinerModel(QObject* parent)
        : QAbstractItemModel(parent)
    {
        
    }

    SceneOutlinerModel::~SceneOutlinerModel()
    {
        
    }

    void SceneOutlinerModel::OnSceneOpened(Scene* scene)
    {
        this->currentScene = scene;
        emit dataChanged(QModelIndex(), QModelIndex());
    }

    void SceneOutlinerModel::OnSceneClosed(Scene *scene)
    {
        if (currentScene == scene)
            this->currentScene = nullptr;
    }

    void SceneOutlinerModel::OnSceneUpdated()
    {
        emit dataChanged(QModelIndex(), QModelIndex());
    }

    QVariant SceneOutlinerModel::data(const QModelIndex &index, int role) const
    {
        if (!index.isValid())
            return QVariant();
        if (role != ::Qt::DisplayRole)
            return QVariant();
        
        GameObject* go = (GameObject*)index.internalPointer();
        if (go == nullptr)
            return QVariant();
        
        return go->GetName().GetCString();
    }

    ::Qt::ItemFlags SceneOutlinerModel::flags(const QModelIndex &index) const
    {
        //if (!index.isValid())
        //    return ::Qt::NoItemFlags;
        
        return ::Qt::ItemFlag::ItemIsSelectable;// & ::Qt::ItemFlag::ItemIsDragEnabled;
    }
    
    QVariant SceneOutlinerModel::headerData(int section, ::Qt::Orientation orientation, int role) const
    {
        return QVariant();
    }

    QModelIndex SceneOutlinerModel::index(int row, int column, const QModelIndex &parent) const
    {
        if (currentScene == nullptr || !hasIndex(row, column))
            return QModelIndex();
        
        if (!parent.isValid())
        {
            return createIndex(row, column, currentScene->GetRootGameObject(row));
        }
        else if (parent.internalPointer() != nullptr)
        {
            GameObject* parentGO = (GameObject*)parent.internalPointer();
            return createIndex(row, column, parentGO->GetChildAt(row));
        }
        
        return QModelIndex();
    }

    QModelIndex SceneOutlinerModel::parent(const QModelIndex &index) const
    {
        if (index.internalPointer() == nullptr)
            return QModelIndex();
        
        GameObject* gameObject = (GameObject*)index.internalPointer();
        if (gameObject->GetParent() == nullptr)
            return QModelIndex();
        return createIndex(gameObject->GetParent()->GetIndexInParent(), 0, gameObject->GetParent());
    }
    
    int SceneOutlinerModel::rowCount(const QModelIndex &parent) const
    {
        if (currentScene == nullptr)
            return 0;
        
        if (!parent.isValid())
            return currentScene->GetRootGameObjectCount();
        
        GameObject* go = (GameObject*)parent.internalPointer();
        return go->GetChildrenCount();
    }
    
    int SceneOutlinerModel::columnCount(const QModelIndex &parent) const
    {
        if (currentScene == nullptr)
            return 0;
        
        return 1;
    }
    
} // namespace CE::Editor

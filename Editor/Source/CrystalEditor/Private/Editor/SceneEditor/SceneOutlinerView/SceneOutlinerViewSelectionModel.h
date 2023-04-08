#ifndef SCENEOUTLINERVIEWSELECTIONMODEL_H
#define SCENEOUTLINERVIEWSELECTIONMODEL_H

#include <QItemSelectionModel>
#include <QWidget>

namespace CE::Edtior
{
    class SceneOutlinerViewSelectionModel : public QItemSelectionModel
    {
        Q_OBJECT
    public:
        SceneOutlinerViewSelectionModel(QAbstractItemModel* parent = nullptr);
        ~SceneOutlinerViewSelectionModel();

    };
}

#endif // SCENEOUTLINERVIEWSELECTIONMODEL_H

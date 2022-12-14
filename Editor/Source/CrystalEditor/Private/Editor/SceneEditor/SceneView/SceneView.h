#ifndef SCENEVIEW_H
#define SCENEVIEW_H

#include <QWidget>

namespace Ui {
class SceneView;
}

namespace CE::Editor
{

    class SceneView : public QWidget
    {
        Q_OBJECT

    public:
        explicit SceneView(QWidget* parent = nullptr);
        ~SceneView();

    private:
        Ui::SceneView* ui;
    };

}

#endif // SCENEVIEW_H

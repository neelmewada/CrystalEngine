#include "SceneView.h"
#include "ui_SceneView.h"

namespace CE::Editor
{

    SceneView::SceneView(QWidget* parent) :
        QWidget(parent),
        ui(new Ui::SceneView)
    {
        ui->setupUi(this);

        setWindowTitle("Scene");
    }

    SceneView::~SceneView()
    {
        delete ui;
    }

}

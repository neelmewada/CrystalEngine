#include "ViewportView.h"
#include "ui_ViewportView.h"

namespace CE::Editor
{

    ViewportView::ViewportView(QWidget* parent)
        : EditorViewBase(parent)
        , ui(new Ui::ViewportView)
    {
        ui->setupUi(this);

        setWindowTitle("Viewport");
    }

    ViewportView::~ViewportView()
    {
        delete ui;
    }

}

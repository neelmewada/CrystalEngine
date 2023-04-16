#ifndef IMAGECANVAS_H
#define IMAGECANVAS_H

#include <QWidget>
#include <QGraphicsView>
#include <QImage>

#include "CoreMedia.h"

namespace Ui {
class ImageCanvas;
}

namespace CE::Editor::Qt
{

    class QTCOMPONENTS_API ImageCanvas : public QWidget
    {
        Q_OBJECT
    public:
        explicit ImageCanvas(QWidget *parent = nullptr);
        ~ImageCanvas();

        void SetImage(const CMImage& image);

        void Recenter();

        void paintEvent(QPaintEvent *event) override;

    private:
        CMImage image{};
        QImage qtImage{};

        QRectF drawRect{};

        Ui::ImageCanvas* ui;
    };

}

#endif // IMAGECANVAS_H

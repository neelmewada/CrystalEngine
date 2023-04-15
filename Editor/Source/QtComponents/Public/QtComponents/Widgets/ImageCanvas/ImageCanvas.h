#ifndef IMAGECANVAS_H
#define IMAGECANVAS_H

#include <QWidget>
#include <QGraphicsView>
#include <QImage>

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

        QGraphicsView* GetGraphicsView();

        void SetImage(const CMImage& image);

        void paintEvent(QPaintEvent *event) override;

    private:
        CMImage image{};
        QImage qtImage{};

        Ui::ImageCanvas *ui;
    };

}

#endif // IMAGECANVAS_H

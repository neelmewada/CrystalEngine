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
    ENUM(Flags)
    enum class ImageCanvasChannel
    {
        None = 0,
        Red = BIT(0),
        Green = BIT(1),
        Blue = BIT(2),
        Alpha = BIT(3),
        All = Red | Green | Blue | Alpha,
    };
    ENUM_CLASS_FLAGS(ImageCanvasChannel)

    class QTCOMPONENTS_API ImageCanvas : public QWidget
    {
        Q_OBJECT
    public:
        explicit ImageCanvas(QWidget *parent = nullptr);
        ~ImageCanvas();

        void SetImage(const CMImage& image);

        void Recenter();

        void paintEvent(QPaintEvent *event) override;

        void SetChannelFilter(ImageCanvasChannel channels);
        ImageCanvasChannel GetChannelFilter() const { return channels; }

    private:
        ImageCanvasChannel channels = ImageCanvasChannel::All;

        CMImage image{};
        QImage qtImage{};

        QRectF drawRect{};

        Ui::ImageCanvas* ui;
    };

}

#include "ImageCanvas.rtti.h"

#endif // IMAGECANVAS_H

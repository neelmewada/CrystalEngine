#include "ImageCanvas.h"
#include "ui_ImageCanvas.h"

#include <QPainter>

namespace CE::Editor::Qt
{

    ImageCanvas::ImageCanvas(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::ImageCanvas)
    {
        ui->setupUi(this);

    }

    ImageCanvas::~ImageCanvas()
    {
        delete ui;
    }

    QGraphicsView* ImageCanvas::GetGraphicsView()
    {
        return ui->graphicsView;
    }

    void ImageCanvas::SetImage(const CMImage& image)
    {
        this->image = image;
        if (!image.IsValid())
        {
            qtImage = QImage();
            return;
        }

        QImage::Format imageFormat{};
        switch (image.GetNumChannels())
        {
            case 1:
                imageFormat = QImage::Format_Grayscale8;
                break;
            case 2:
                imageFormat = QImage::Format_Grayscale16;
                break;
            case 3:
                imageFormat = QImage::Format_RGB888;
                break;
            case 4:
                imageFormat = QImage::Format_RGBA8888;
                break;
        }

        qtImage = QImage(image.GetDataPtr(), image.GetWidth(), image.GetHeight(), imageFormat);
        repaint();
    }

    void ImageCanvas::paintEvent(QPaintEvent* event)
    {
        QWidget::paintEvent(event);

        if (qtImage.isNull())
            return;

        QPainter painter(this);

        painter.drawImage(QPoint(0, 0), qtImage);
    }

}

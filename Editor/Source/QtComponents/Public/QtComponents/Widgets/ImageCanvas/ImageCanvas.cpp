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
        Recenter();
    }

    void ImageCanvas::paintEvent(QPaintEvent* event)
    {
        QWidget::paintEvent(event);

        if (qtImage.isNull())
            return;

        QPainter painter(this);
        painter.setBackgroundMode(::Qt::OpaqueMode);
        painter.setBrush(QBrush(QColor(10, 10, 10)));
        painter.setPen(::Qt::NoPen);
        painter.drawRect(0, 0, width(), height());

        QColor color = QColor(0, 0, 0, 255);

        if (EnumHasFlag(channels, ImageCanvasChannel::Red))
            color.setRed(255);
        if (EnumHasFlag(channels, ImageCanvasChannel::Green))
            color.setGreen(255);
        if (EnumHasFlag(channels, ImageCanvasChannel::Blue))
            color.setBlue(255);
        if (EnumHasFlag(channels, ImageCanvasChannel::Alpha))
        {
            color.setAlpha(255);
            if (channels == ImageCanvasChannel::Alpha)
                color = QColor(255, 255, 255, 255);
        }

        painter.fillRect(drawRect, color);

        if (channels == ImageCanvasChannel::None)
            return;

        painter.setCompositionMode(QPainter::CompositionMode_Multiply);
        painter.drawImage(drawRect, qtImage);
    }

    void ImageCanvas::Recenter()
    {
        if (qtImage.isNull())
            return;

        auto maxImageSize = Math::Min((f32)this->width(), (f32)this->height()) * 0.75f;
        auto actualImageSize = Math::Max((f32)qtImage.width(), (f32)qtImage.height());
        f32 scale = maxImageSize / actualImageSize;

        drawRect.setTopLeft(QPointF((f32)this->width() / 2.0f - qtImage.width() * scale / 2.0f, (f32)this->height() / 2.0f - qtImage.height() * scale / 2.0f));
        drawRect.setSize(QSizeF(qtImage.width() * scale, qtImage.height() * scale));

        repaint();
    }

    void ImageCanvas::SetChannelFilter(ImageCanvasChannel channels)
    {
        this->channels = channels;
        repaint();
    }

}

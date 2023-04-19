#ifndef TEXTUREPREVIEWVIEW_H
#define TEXTUREPREVIEWVIEW_H

#include <QWidget>

#include "Editor/EditorView.h"

#include "CoreMedia.h"
#include "QtComponents.h"

namespace Ui {
class TexturePreviewView;
}

namespace CE::Editor
{

    CLASS()
    class TexturePreviewView : public EditorView
    {
        Q_OBJECT
        CE_CLASS(TexturePreviewView, CE::Editor::EditorView)
    public:
        explicit TexturePreviewView(QWidget* parent = nullptr);
        ~TexturePreviewView();

        void SetImage(const CMImage& image);

        void Recenter();

        void RefreshPreview();

        void resizeEvent(QResizeEvent *event) override;

        void SetImageCanvasChannel(Qt::ImageCanvasChannel channelFlags);

        Qt::ImageCanvasChannel GetImageCanvasChannelFlags();

    private:
        Ui::TexturePreviewView* ui;
    };

}

#include "TexturePreviewView.rtti.h"

#endif // TEXTUREPREVIEWVIEW_H

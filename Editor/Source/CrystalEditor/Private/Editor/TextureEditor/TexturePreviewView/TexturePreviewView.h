#ifndef TEXTUREPREVIEWVIEW_H
#define TEXTUREPREVIEWVIEW_H

#include <QWidget>

#include "Editor/EditorViewBase.h"

#include "CoreMedia.h"

namespace Ui {
class TexturePreviewView;
}

namespace CE::Editor
{

    CLASS()
    class TexturePreviewView : public EditorViewBase
    {
        Q_OBJECT
        CE_CLASS(TexturePreviewView, CE::Editor::EditorViewBase)
    public:
        explicit TexturePreviewView(QWidget* parent = nullptr);
        ~TexturePreviewView();

        void SetImage(const CMImage& image);

        void Recenter();

        void RefreshPreview();

        void resizeEvent(QResizeEvent *event) override;

    private:
        Ui::TexturePreviewView* ui;
    };

}

#include "TexturePreviewView.rtti.h"

#endif // TEXTUREPREVIEWVIEW_H

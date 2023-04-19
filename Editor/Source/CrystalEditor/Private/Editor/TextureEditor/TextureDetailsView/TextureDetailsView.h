#ifndef TEXTUREDETAILSVIEW_H
#define TEXTUREDETAILSVIEW_H

#include <QWidget>

#include "Editor/EditorView.h"

namespace Ui {
class TextureDetailsView;
}

namespace CE::Editor
{
    CLASS()
    class TextureDetailsView : public EditorView
    {
        Q_OBJECT
        CE_CLASS(TextureDetailsView, CE::Editor::EditorView)
    public:
        explicit TextureDetailsView(QWidget* parent = nullptr);
        ~TextureDetailsView();

    private:
        Ui::TextureDetailsView* ui;
    };

}

#include "TextureDetailsView.rtti.h"

#endif // TEXTUREDETAILSVIEW_H

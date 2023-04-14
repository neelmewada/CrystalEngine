#ifndef TEXTUREEDITORWINDOW_H
#define TEXTUREEDITORWINDOW_H

#include <QMainWindow>

namespace Ui {
class TextureEditorWindow;
}

namespace CE::Editor
{

    class TextureEditorWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        explicit TextureEditorWindow(QWidget *parent = nullptr);
        ~TextureEditorWindow();

    private:
        Ui::TextureEditorWindow *ui;
    };

}

#endif // TEXTUREEDITORWINDOW_H

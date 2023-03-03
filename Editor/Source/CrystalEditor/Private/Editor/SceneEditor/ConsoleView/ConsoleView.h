#ifndef CONSOLEVIEW_H
#define CONSOLEVIEW_H

#include <QWidget>
#include <QString>

#include "CoreMinimal.h"
#include "GameFramework/GameFramework.h"

namespace Ui {
class ConsoleView;
}

namespace CE::Editor
{
    class ConsoleViewModel;

    class ConsoleView : public EditorViewBase
    {
        Q_OBJECT
        CE_CLASS(ConsoleView, EditorViewBase)
    public:
        explicit ConsoleView(QWidget *parent = nullptr);
        ~ConsoleView();

    private:
        void Log(const String& string);
        void Flush();

    private slots:
        void LogSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

        void UpdateLogSelection();

        void on_searchInput_textEdited(const QString& searchText);

        void on_clearButton_clicked();

    private:
        Ui::ConsoleView *ui;
        ConsoleViewModel* model = nullptr;

        template<typename Mutex>
        friend class EditorConsoleSink;
    };

}


CE_RTTI_CLASS(CRYSTALEDITOR_API, CE::Editor, ConsoleView,
    CE_SUPER(CE::Editor::EditorViewBase),
    CE_DONT_INSTANTIATE,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST()
)

#endif // CONSOLEVIEW_H

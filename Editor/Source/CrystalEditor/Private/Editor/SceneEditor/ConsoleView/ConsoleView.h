#ifndef CONSOLEVIEW_H
#define CONSOLEVIEW_H

#include <QWidget>
#include <QString>
#include <QItemSelection>

#include "CoreMinimal.h"
#include "GameFramework/GameFramework.h"

#include <mutex>

namespace Ui {
class ConsoleView;
}

namespace CE::Editor
{
    class ConsoleViewModel;

    class ConsoleView : public EditorView
    {
        Q_OBJECT
        CE_CLASS(ConsoleView, CE::Editor::EditorView)
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

    signals:
        void OnLogPushed(const String& string);

    private:
        Ui::ConsoleView *ui;
        ConsoleViewModel* model = nullptr;


        std::mutex mut{};

        template<typename Mutex>
        friend class EditorConsoleSink;
    };

}


CE_RTTI_CLASS(CRYSTALEDITOR_API, CE::Editor, ConsoleView,
    CE_SUPER(CE::Editor::EditorView),
    CE_DONT_INSTANTIATE,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST()
)

#endif // CONSOLEVIEW_H

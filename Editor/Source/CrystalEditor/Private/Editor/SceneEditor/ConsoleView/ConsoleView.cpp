#include "ConsoleView.h"
#include "ui_ConsoleView.h"

#include "ConsoleViewModel.h"

#include "spdlog/spdlog.h"
#include "spdlog/logger.h"
#include "spdlog/sinks/base_sink.h"

#include "spdlog/details/null_mutex.h"
#include <mutex>

namespace CE::Editor
{


    template<typename Mutex>
    class EditorConsoleSink : public spdlog::sinks::base_sink<Mutex>
    {
    public:
        EditorConsoleSink(ConsoleView* owner) : owner(owner)
        {
            
        }

    protected:
        void sink_it_(const spdlog::details::log_msg& msg) override
        {
            // log_msg is a struct containing the log entry info like level, timestamp, thread id etc.
            // msg.raw contains pre formatted log

            // If needed (very likely but not mandatory), the sink formats the message before sending it to its final destination:
            spdlog::memory_buf_t formatted;
            spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);
            auto str = fmt::to_string(formatted);

            if (owner != nullptr)
                owner->Log(str);
        }

        void flush_() override
        {
            if (owner != nullptr)
                owner->Flush();
        }

        ConsoleView* owner = nullptr;
    };

    using EditorConsoleSinkMt = EditorConsoleSink<std::mutex>;
    using EditorConsoleSinkSt = EditorConsoleSink<spdlog::details::null_mutex>;

    static std::shared_ptr<EditorConsoleSinkMt> ConsoleSink{};
    static std::shared_ptr<spdlog::logger> EditorConsoleLogger{};

    ConsoleView::ConsoleView(QWidget *parent) :
        EditorViewBase(parent),
        ui(new Ui::ConsoleView)
    {
        ui->setupUi(this);

        // UI setup
        setWindowTitle("Console");
        ui->clearButton->setIcon(QIcon(":/Editor/Icons/bin-red"));

        // Model setup
        model = new ConsoleViewModel(this);
        ui->consoleOutput->setModel(model);
        connect(ui->consoleOutput->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ConsoleView::LogSelectionChanged);
        connect(model, &ConsoleViewModel::ShouldRefreshSelection, this, &ConsoleView::UpdateLogSelection);

        // Logger setup
        ConsoleSink = std::make_shared<EditorConsoleSinkMt>(this);
        EditorConsoleLogger = std::make_shared<spdlog::logger>("EditorConsole", spdlog::sinks_init_list{ ConsoleSink });

        CE::Logger::SetEditorLogger(EditorConsoleLogger.get());
        CE::Logger::SetEditorLoggerPattern("[%^%l%$] %v");
    }

    ConsoleView::~ConsoleView()
    {
        delete ui;

        CE::Logger::SetEditorLogger(nullptr);

        EditorConsoleLogger.reset();
        ConsoleSink.reset();
    }

    void ConsoleView::Log(const String& string)
    {
        model->PushEntry(string);
        ui->consoleOutput->scrollToBottom();
    }

    void ConsoleView::Flush()
    {
    }

    void ConsoleView::LogSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
    {
        Q_UNUSED(selected);
        Q_UNUSED(deselected);

        UpdateLogSelection();
    }

    void ConsoleView::UpdateLogSelection()
    {
        auto selection = ui->consoleOutput->selectionModel()->selectedIndexes();
        if (selection.count() == 0)
        {
            ui->logDetails->setText("");
            return;
        }

        const auto& index = selection.at(0);

        String details = model->GetEntryDetailsFor(index);

        ui->logDetails->setText(QString(details.GetCString()));
    }

    void ConsoleView::on_searchInput_textEdited(const QString& searchText)
    {
        auto searchFilterQString = ui->searchInput->text().toStdString();
        model->SetSearchFilter(String(searchFilterQString.c_str()));
    }

    void ConsoleView::on_clearButton_clicked()
    {
        model->ClearLogs();
    }

}

CE_RTTI_CLASS_IMPL(CRYSTALEDITOR_API, CE::Editor, ConsoleView)


#include "CoreTypes.h"

#include "Logger/Logger.h"

#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#if PLATFORM_WINDOWS
#include "spdlog/sinks/wincolor_sink.h"
#endif
#include "spdlog/sinks/basic_file_sink.h"

#include "PAL/Common/PlatformDirectories.h"


namespace CE
{
    static std::shared_ptr<spdlog::sinks::stdout_color_sink_st> GSystemConsoleSink;
    static std::shared_ptr<spdlog::sinks::basic_file_sink_st> GLogFileSink;

    static std::shared_ptr<spdlog::logger> GConsoleLogger;
    static std::shared_ptr<spdlog::logger> GFileLogger;

    static spdlog::logger* EditorLogger = nullptr;

    static bool GIsLoggerInitialized = false;
    static CE::LogLevel GConsoleLogLevel = CE::LogLevel::Trace;
    static CE::LogLevel GFileDumpLogLevel = CE::LogLevel::Trace;

    void Logger::SetConsoleLogLevel(LogLevel level)
    {
        GConsoleLogLevel = level;

        if (GIsLoggerInitialized)
        {
            GSystemConsoleSink->set_level((spdlog::level::level_enum)GConsoleLogLevel);
        }
    }

    void Logger::SetFileDumpLogLevel(LogLevel level)
    {
        GFileDumpLogLevel = level;

        if (GIsLoggerInitialized)
        {
            GLogFileSink->set_level((spdlog::level::level_enum)GFileDumpLogLevel);
        }
    }

    void Logger::Initialize()
    {
        spdlog::init_thread_pool(8192, 1);

        GSystemConsoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_st>();
        GSystemConsoleSink->set_level((spdlog::level::level_enum)GConsoleLogLevel);
        GSystemConsoleSink->set_pattern("[%^%l%$] %v");

        auto logPath = (PlatformDirectories::GetLogDir() / "log.txt").GetString();

        GLogFileSink = std::make_shared<spdlog::sinks::basic_file_sink_st>(logPath.GetCString());
        GLogFileSink->set_level((spdlog::level::level_enum)GFileDumpLogLevel);
        GLogFileSink->set_pattern("[%Y-%m-%d %T] [%l] %v");

        std::vector<spdlog::sink_ptr> consoleSinks{ GSystemConsoleSink };
        
        GConsoleLogger = std::make_shared<spdlog::logger>("ConsoleLog", consoleSinks.begin(), consoleSinks.end());
        GFileLogger = std::make_shared<spdlog::logger>("FileLog", GLogFileSink);
        
        GIsLoggerInitialized = true;
    }

    void Logger::Shutdown()
    {
        GSystemConsoleSink.reset();
        GLogFileSink.reset();
        GConsoleLogger.reset();
        GFileLogger.reset();

        GIsLoggerInitialized = false;
    }

    void Logger::Log(LogLevel level, StringView message, LogTarget target)
    {
        if (!GIsLoggerInitialized)
            return;

        if (EnumHasAnyFlags(target, LogTarget::Console))
        {
            GConsoleLogger->log((spdlog::level::level_enum)level, message.GetCString());

            if (EditorLogger != nullptr)
            {
                EditorLogger->log((spdlog::level::level_enum)level, message.GetCString());
            }
        }

        if (EnumHasAnyFlags(target, LogTarget::LogFile))
        {
            GFileLogger->log((spdlog::level::level_enum)level, message.GetCString());
        }
    }

    void Logger::Log(LogLevel level, StringView message, const char* fileName, int line, LogTarget target)
    {
        if (!GIsLoggerInitialized)
            return;

        String fullMsg = message;

#if !CE_BUILD_RELEASE // For Non-release builds
        if (level == LogLevel::Error || level == LogLevel::Critical)
        {
            fullMsg = String::Format("{}\n{} Line {}", message, fileName, line);
        }
#endif

        Log(level, fullMsg, target);
    }

    std::shared_ptr<spdlog::logger> Logger::GetConsoleLogger()
    {
        return GConsoleLogger;
    }

    void Logger::SetEditorLogger(spdlog::logger* logger)
    {
        EditorLogger = logger;
    }

    void Logger::SetEditorLoggerPattern(String pattern)
    {
        if (EditorLogger == nullptr)
            return;

        EditorLogger->set_pattern(pattern);
    }

} // namespace CE

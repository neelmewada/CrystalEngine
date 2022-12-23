#pragma once

#if CE_BUILD_RELEASE // Don't show line no. & file name in Release build logs
#   define CE_LOG(logLevel, target, msg, ...) CE::Logger::Log(CE::LogLevel::logLevel, CE::String::Format(msg, ##__VA_ARGS__), CE::LogTarget::target)
#else
#   define CE_LOG(logLevel, target, msg, ...) CE::Logger::Log(CE::LogLevel::logLevel, CE::String::Format(msg, ##__VA_ARGS__), __FILE__, __LINE__, CE::LogTarget::target)
#endif

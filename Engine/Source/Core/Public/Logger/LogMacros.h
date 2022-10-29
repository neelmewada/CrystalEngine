#pragma once

#define CE_LOG(logLevel, msg, ...) CE::Logger::Log(CE::LogLevel::logLevel, CE::String::Format(msg, ##__VA_ARGS__), __FILE__, __LINE__)


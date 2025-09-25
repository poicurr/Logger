#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>

namespace logger {

enum class LogLevel {
  Trace = 0,
  Debug = 1,
  Info = 2,
  Warn = 3,
  Error = 4,
  Fatal = 5
};

class Logger {
public:
  Logger() = delete;

  static void SetOutputStream(std::ostream &stream) {
    std::lock_guard<std::mutex> guard(m_mutex);
    m_outputStream = &stream;
  }

  static void SetLevel(LogLevel level) {
    std::lock_guard<std::mutex> guard(m_mutex);
    m_level = level;
  }

  static LogLevel GetLevel() {
    std::lock_guard<std::mutex> guard(m_mutex);
    return m_level;
  }

  static void EnableTimestamp(bool enable) {
    std::lock_guard<std::mutex> guard(m_mutex);
    m_timestampEnabled = enable;
  }

  static bool IsTimestampEnabled() {
    std::lock_guard<std::mutex> guard(m_mutex);
    return m_timestampEnabled;
  }

  template <typename... Args>
  static void Log(LogLevel level, std::string_view format,
                  const Args &...args) {
    if (!ShouldLog(level))
      return;

    const std::string message = FormatMessage(format, args...);

    std::lock_guard<std::mutex> guard(m_mutex);
    auto &stream = *m_outputStream;
    if (m_timestampEnabled) {
      stream << FormatTimestamp() << ' ';
    }
    stream << '[' << LevelToString(level) << "] " << message << '\n';
    stream.flush();
  }

  template <typename... Args>
  static void Trace(std::string_view format, const Args &...args) {
    Log(LogLevel::Trace, format, args...);
  }

  template <typename... Args>
  static void Debug(std::string_view format, const Args &...args) {
    Log(LogLevel::Debug, format, args...);
  }

  template <typename... Args>
  static void Info(std::string_view format, const Args &...args) {
    Log(LogLevel::Info, format, args...);
  }

  template <typename... Args>
  static void Warn(std::string_view format, const Args &...args) {
    Log(LogLevel::Warn, format, args...);
  }

  template <typename... Args>
  static void Error(std::string_view format, const Args &...args) {
    Log(LogLevel::Error, format, args...);
  }

  template <typename... Args>
  static void Fatal(std::string_view format, const Args &...args) {
    Log(LogLevel::Fatal, format, args...);
  }

private:
  static bool ShouldLog(LogLevel level) {
    std::lock_guard<std::mutex> guard(m_mutex);
    return static_cast<int>(level) >= static_cast<int>(m_level);
  }

  static std::string FormatTimestamp() {
    using clock = std::chrono::system_clock;
    const auto now = clock::now();
    const std::time_t time = clock::to_time_t(now);

#if defined(_MSC_VER)
    std::tm localTime{};
    localtime_s(&localTime, &time);
#else
    std::tm localTime{};
    localtime_r(&time, &localTime);
#endif

    std::ostringstream stream;
    stream << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
    return stream.str();
  }

  static std::string_view LevelToString(LogLevel level) {
    switch (level) {
    case LogLevel::Trace:
      return "TRACE";
    case LogLevel::Debug:
      return "DEBUG";
    case LogLevel::Info:
      return "INFO";
    case LogLevel::Warn:
      return "WARN";
    case LogLevel::Error:
      return "ERROR";
    case LogLevel::Fatal:
      return "FATAL";
    }
    return "INFO";
  }

  template <typename... Args>
  static std::string FormatMessage(std::string_view format,
                                   const Args &...args) {
    std::ostringstream stream;
    AppendFormatted(stream, format, args...);
    return stream.str();
  }

  template <typename Arg, typename... Rest>
  static void AppendFormatted(std::ostringstream &stream,
                              std::string_view format, const Arg &arg,
                              const Rest &...rest) {
    const std::size_t placeholder = format.find("{}");
    if (placeholder == std::string_view::npos) {
      stream << format;
      return;
    }

    stream << format.substr(0, placeholder);
    stream << arg;
    AppendFormatted(stream, format.substr(placeholder + 2), rest...);
  }

  static void AppendFormatted(std::ostringstream &stream,
                              std::string_view text) {
    stream << text;
  }

  inline static std::ostream *m_outputStream = &std::clog;
  inline static LogLevel m_level = LogLevel::Info;
  inline static bool m_timestampEnabled = true;
  inline static std::mutex m_mutex;
};

} // namespace logger

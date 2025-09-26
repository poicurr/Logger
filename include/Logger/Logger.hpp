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

  static void setOutputStream(std::ostream &stream) {
    std::lock_guard<std::mutex> guard(s_mutex);
    s_outputStream = &stream;
  }

  static void setLevel(LogLevel level) {
    std::lock_guard<std::mutex> guard(s_mutex);
    s_level = level;
  }

  static LogLevel getLevel() {
    std::lock_guard<std::mutex> guard(s_mutex);
    return s_level;
  }

  static void enableTimestamp(bool enable) {
    std::lock_guard<std::mutex> guard(s_mutex);
    s_timestampEnabled = enable;
  }

  static bool isTimestampEnabled() {
    std::lock_guard<std::mutex> guard(s_mutex);
    return s_timestampEnabled;
  }

  template <typename... Args>
  static void log(LogLevel level, std::string_view format,
                  const Args &...args) {
    if (!shouldLog(level))
      return;

    const std::string message = formatMessage(format, args...);

    std::lock_guard<std::mutex> guard(s_mutex);
    auto &stream = *s_outputStream;
    if (s_timestampEnabled) {
      stream << formatTimestamp() << ' ';
    }
    stream << '[' << levelStr(level) << "] " << message << '\n';
    stream.flush();
  }

  template <typename... Args>
  static void trace(std::string_view format, const Args &...args) {
    log(LogLevel::Trace, format, args...);
  }

  template <typename... Args>
  static void debug(std::string_view format, const Args &...args) {
    log(LogLevel::Debug, format, args...);
  }

  template <typename... Args>
  static void info(std::string_view format, const Args &...args) {
    log(LogLevel::Info, format, args...);
  }

  template <typename... Args>
  static void warn(std::string_view format, const Args &...args) {
    log(LogLevel::Warn, format, args...);
  }

  template <typename... Args>
  static void error(std::string_view format, const Args &...args) {
    log(LogLevel::Error, format, args...);
  }

  template <typename... Args>
  static void fatal(std::string_view format, const Args &...args) {
    log(LogLevel::Fatal, format, args...);
  }

private:
  static bool shouldLog(LogLevel level) {
    std::lock_guard<std::mutex> guard(s_mutex);
    return static_cast<int>(level) >= static_cast<int>(s_level);
  }

  static std::string formatTimestamp() {
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
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  now.time_since_epoch()) %
              1000;

    std::ostringstream stream;
    stream << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
    stream << '.' << std::setw(3) << std::setfill('0') << ms.count();
    return stream.str();
  }

  static std::string_view levelStr(LogLevel level) {
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
  static std::string formatMessage(std::string_view format,
                                   const Args &...args) {
    std::ostringstream stream;
    appendFormatted(stream, format, args...);
    return stream.str();
  }

  template <typename Arg, typename... Rest>
  static void appendFormatted(std::ostringstream &stream,
                              std::string_view format, const Arg &arg,
                              const Rest &...rest) {
    const std::size_t placeholder = format.find("{}");
    if (placeholder == std::string_view::npos) {
      stream << format;
      return;
    }

    stream << format.substr(0, placeholder);
    stream << arg;
    appendFormatted(stream, format.substr(placeholder + 2), rest...);
  }

  static void appendFormatted(std::ostringstream &stream,
                              std::string_view text) {
    stream << text;
  }

  inline static std::ostream *s_outputStream = &std::clog;
  inline static LogLevel s_level = LogLevel::Info;
  inline static bool s_timestampEnabled = true;
  inline static std::mutex s_mutex;
};

} // namespace logger

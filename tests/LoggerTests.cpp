#include "Logger/Logger.hpp"

#include <cassert>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

using logger::Logger;
using logger::LogLevel;

int main() {
  {
    std::ostringstream stream;
    Logger::SetOutputStream(stream);
    Logger::SetLevel(LogLevel::Trace);
    Logger::EnableTimestamp(false);

    Logger::Info("Hello {}", "world");
    Logger::SetOutputStream(std::clog);

    const std::string output = stream.str();
    assert(output.find("[INFO] Hello world") != std::string::npos);
    assert(!output.empty() && output.front() == '[');
  }

  {
    std::ostringstream stream;
    Logger::SetOutputStream(stream);
    Logger::SetLevel(LogLevel::Warn);
    Logger::EnableTimestamp(false);

    Logger::Info("Ignored message");
    Logger::Error("An error {}", 42);
    Logger::SetOutputStream(std::clog);

    const std::string output = stream.str();
    assert(output.find("Ignored message") == std::string::npos);
    assert(output.find("[ERROR] An error 42") != std::string::npos);
  }

  {
    std::ostringstream stream;
    Logger::SetOutputStream(stream);
    Logger::SetLevel(LogLevel::Debug);
    Logger::EnableTimestamp(true);

    Logger::Debug("Coordinates {} {}", 10, 20);
    Logger::SetOutputStream(std::clog);

    const std::string output = stream.str();
    assert(!output.empty() &&
           std::isdigit(static_cast<unsigned char>(output.front())));
    assert(output.find("[DEBUG] Coordinates 10 20") != std::string::npos);
  }

  {
    namespace fs = std::filesystem;
    const fs::path logPath =
        fs::temp_directory_path() / "logger_multithread_test.log";
    const std::size_t threadCount = 4;
    const std::size_t messagesPerThread = 25;

    {
      std::ofstream file(logPath, std::ios::trunc);
      assert(file.is_open());

      Logger::SetOutputStream(file);
      Logger::SetLevel(LogLevel::Info);
      Logger::EnableTimestamp(false);

      std::vector<std::thread> workers;
      workers.reserve(threadCount);
      for (std::size_t i = 0; i < threadCount; ++i) {
        workers.emplace_back([i, messagesPerThread]() {
          for (std::size_t j = 0; j < messagesPerThread; ++j) {
            Logger::Info("thread {} message {}", i, j);
          }
        });
      }

      for (auto &worker : workers) {
        worker.join();
      }

      Logger::SetOutputStream(std::clog);
    }

    std::ifstream input(logPath);
    assert(input.is_open());

    std::vector<std::string> prefixes;
    prefixes.reserve(threadCount);
    for (std::size_t i = 0; i < threadCount; ++i) {
      prefixes.emplace_back("[INFO] thread " + std::to_string(i) + " message ");
    }

    std::vector<std::size_t> counts(threadCount, 0);
    std::size_t lineCount = 0;
    std::string line;
    while (std::getline(input, line)) {
      if (line.empty()) {
        continue;
      }
      ++lineCount;
      for (std::size_t i = 0; i < prefixes.size(); ++i) {
        if (line.find(prefixes[i]) != std::string::npos) {
          ++counts[i];
          break;
        }
      }
    }

    input.close();
    fs::remove(logPath);

    assert(lineCount == threadCount * messagesPerThread);
    for (std::size_t i = 0; i < counts.size(); ++i) {
      assert(counts[i] == messagesPerThread);
    }
  }

  Logger::SetOutputStream(std::clog);
  Logger::SetLevel(LogLevel::Info);
  Logger::EnableTimestamp(true);

  return 0;
}

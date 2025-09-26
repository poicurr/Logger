# Logger

A lightweight, header-only Logger library.
Available via `#include <Logger/Logger.hpp>`.

## Features

- Header-only (no linking required)
- Easily integrated using `FetchContent_Declare` or `add_subdirectory`

## Usage (CMake)

### Example using FetchContent

```cmake
include(FetchContent)
FetchContent_Declare(
  Logger
  GIT_REPOSITORY https://github.com/poicurr/Logger.git
  GIT_TAG main)

FetchContent_MakeAvailable(Logger)

target_link_libraries(MyApp PRIVATE Logger)
```

### Include Example

```cpp
#include <fstream>
#include <Logger/Logger.hpp>

using logger::Logger;
using logger::LogLevel;

int main() {
  std::ofstream file("./test.log", std::ios::trunc);

  Logger::setOutputStream(file);
  Logger::setLevel(LogLevel::Warn);
  Logger::enableTimestamp(true);

  Logger::info("Ignored message");
  Logger::error("An error {}", 42);
}
```

## Enabling Tests (Optional)

By default, tests are not built. To enable tests:

```cmake
set(LOGGER_ENABLE_TESTS ON CACHE BOOL “” FORCE)
FetchContent_MakeAvailable(Logger)
```

Afterwards, you can run them via CTest or manually:

```sh
ctest    # Via CTest
./logger_tests  # Manual execution
```

## License

MIT License


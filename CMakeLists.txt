cmake_minimum_required(VERSION 3.15)
project(devpulse VERSION 1.0.0 LANGUAGES C CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# C Standard
set(CMAKE_C_STANDARD 11)

# Find packages
find_package(ftxui REQUIRED COMPONENTS screen dom component)

# nlohmann/json
find_package(nlohmann_json REQUIRED)

# Collect all C source files
file(GLOB_RECURSE C_SOURCES "src/core/*.c")

# Collect all C++ source files
file(GLOB_RECURSE CXX_SOURCES
    "src/*.cpp"
    "src/panels/*.cpp"
    "src/core/*.cpp"
    "src/utils/*.cpp"
)

# Build executable
add_executable(devpulse ${CXX_SOURCES} ${C_SOURCES})

# Include directories
target_include_directories(devpulse PRIVATE
    src/
    src/panels/
    src/core/
    src/utils/
    include/
)

# Link libraries
target_link_libraries(devpulse PRIVATE
    ftxui::screen
    ftxui::dom
    ftxui::component
    nlohmann_json::nlohmann_json
)

# Compiler warnings
target_compile_options(devpulse PRIVATE -Wall -Wextra -O2)

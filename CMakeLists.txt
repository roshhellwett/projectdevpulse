cmake_minimum_required(VERSION 3.15)
project(devpulse VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -DWIN32_LEAN_AND_MEAN)
    set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc")
endif()

include(FetchContent)
FetchContent_Declare(
    ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
    GIT_TAG v5.0.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(ftxui)

file(GLOB_RECURSE CXX_SOURCES
    "src/*.cpp"
    "src/panels/*.cpp"
    "src/core/*.cpp"
    "src/utils/*.cpp"
)

if(UNIX AND NOT APPLE)
    list(FILTER CXX_SOURCES EXCLUDE REGEX ".*sysreader_win\\.cpp$")
endif()

add_executable(devpulse ${CXX_SOURCES})

target_include_directories(devpulse PRIVATE
    src/
    src/panels/
    src/core/
    src/utils/
    include/
)

target_link_libraries(devpulse PRIVATE ftxui::component ftxui::dom ftxui::screen)

if(WIN32)
    target_link_libraries(devpulse PRIVATE psapi kernel32)
endif()

target_compile_options(devpulse PRIVATE -Wall -Wextra -O2)

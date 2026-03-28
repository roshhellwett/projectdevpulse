cmake_minimum_required(VERSION 3.15)
project(devpulse VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

if(WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -DWIN32_LEAN_AND_MEAN)
    set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc")
endif()

file(GLOB_RECURSE C_SOURCES "src/core/*.c")
file(GLOB_RECURSE CXX_SOURCES
    "src/*.cpp"
    "src/panels/*.cpp"
    "src/core/*.cpp"
    "src/utils/*.cpp"
)

if(WIN32)
    list(FILTER C_SOURCES EXCLUDE REGEX ".*sysreader\\.c$")
endif()
if(UNIX AND NOT APPLE)
    list(FILTER CXX_SOURCES EXCLUDE REGEX ".*sysreader_win\\.cpp$")
endif()

add_executable(devpulse ${CXX_SOURCES} ${C_SOURCES})

target_include_directories(devpulse PRIVATE
    src/
    src/panels/
    src/core/
    src/utils/
    include/
    C:/msys64/mingw64/include
    C:/Users/roshh/AppData/Local/Temp/ftxui/include
)

target_link_libraries(devpulse PRIVATE
    C:/Users/roshh/AppData/Local/Temp/ftxui/build_lib/libftxui-component.a
    C:/Users/roshh/AppData/Local/Temp/ftxui/build_lib/libftxui-dom.a
    C:/Users/roshh/AppData/Local/Temp/ftxui/build_lib/libftxui-screen.a
    pthread
    psapi
    kernel32
)

target_compile_options(devpulse PRIVATE -Wall -Wextra -O2)

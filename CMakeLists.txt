cmake_minimum_required(VERSION 3.16)
project(anima LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(CURL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_library(imgui STATIC
    third_party/imgui/imgui.cpp
    third_party/imgui/imgui_draw.cpp
    third_party/imgui/imgui_tables.cpp
    third_party/imgui/imgui_widgets.cpp
    third_party/imgui/imgui_demo.cpp
    third_party/imgui/backends/imgui_impl_glfw.cpp
    third_party/imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
    third_party/imgui
    third_party/imgui/backends
)

add_library(sqlite3 STATIC third_party/sqlite/sqlite3.c)
target_include_directories(sqlite3 PUBLIC third_party/sqlite3)

target_link_libraries(imgui PUBLIC glfw OpenGL::GL)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(anima 
                src/main.cpp 
                src/LlmClient.cpp
                src/Persona.cpp
                src/WorkingMemory.cpp
                src/Mood.cpp
            )

target_link_libraries(anima PRIVATE imgui CURL::libcurl nlohmann_json::nlohmann_json sqlite3)
target_compile_options(anima PRIVATE -Wall -Wextra)


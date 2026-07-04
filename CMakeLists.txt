cmake_minimum_required(VERSION 3.14)
project(gak CXX)

set(CMAKE_CXX_STANDARD 17)

# Automatically download FTXUI from GitHub
include(FetchContent)
FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
  GIT_TAG main # Or a specific version tag like v5.0.0
)
FetchContent_MakeAvailable(ftxui)

# Define your executable
add_executable(gak src/css/test.cpp)

target_sources(gak PRIVATE
    src/css/lexer.cpp
    src/css/parser.cpp
)

# Link the required FTXUI libraries
target_link_libraries(gak PRIVATE 
    ftxui::screen 
    ftxui::dom 
    ftxui::component
)
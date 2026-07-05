cmake_minimum_required(VERSION 3.14)
project(gak CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenSSL REQUIRED)

# Automatically download FTXUI from GitHub
include(FetchContent)
FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
  GIT_TAG main # Or a specific version tag like v5.0.0
)
FetchContent_MakeAvailable(ftxui)

add_executable(gak src/main.cpp)

target_include_directories(gak PRIVATE include)
target_compile_definitions(gak PRIVATE CPPHTTPLIB_OPENSSL_SUPPORT)
target_link_libraries(gak PRIVATE OpenSSL::SSL OpenSSL::Crypto)

set_target_properties(gak PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_sources(gak PRIVATE
    src/css/lexer.cpp
    src/css/parser.cpp
    src/renderer/dom_node.cpp
    src/renderer/dom_node.h
    src/renderer/html_parser.cpp
    src/renderer/html_parser.h
    src/renderer/renderer.cpp
    src/renderer/renderer.h
)

target_include_directories(gak PRIVATE 
    src
    src/renderer
    src/css
)

# Link the required FTXUI libraries
target_link_libraries(gak PRIVATE 
    ftxui::screen 
    ftxui::dom 
    ftxui::component
)
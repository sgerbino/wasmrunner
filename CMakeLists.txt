cmake_minimum_required(VERSION 3.19.0)
cmake_policy(VERSION 3.19.0...3.31.6)

set(HUNTER_PACKAGES zstd libxml2 ZLIB LLVM)
set(HUNTER_FILEPATH_CONFIG ${CMAKE_CURRENT_LIST_DIR}/data/hunter/config.cmake)

include(FetchContent)
FetchContent_Declare(SetupHunter GIT_REPOSITORY https://github.com/cpp-pm/gate)
FetchContent_MakeAvailable(SetupHunter)

project(wasmrunner
  VERSION 0.0.1
  DESCRIPTION "An embedded WASM runner"
  LANGUAGES CXX C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(zstd CONFIG REQUIRED)
find_package(libxml2 CONFIG REQUIRED)
find_Package(ZLIB CONFIG REQUIRED)
find_package(LLVM CONFIG REQUIRED)

add_subdirectory(external)
add_subdirectory(src)


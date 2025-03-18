cmake_minimum_required(VERSION 3.19.0)
cmake_policy(VERSION 3.19.0...3.27.4)

file(WRITE ${CMAKE_BINARY_DIR}/config.cmake [=[
hunter_config(iwasm
  URL "https://github.com/bytecodealliance/wasm-micro-runtime/archive/c30e65ba5d2bb4c1b96e23dfaa74e498fc3ac3a3.tar.gz"
  SHA1 "05eaf0c7aa751b775482498a2a2563466a6ebb49"
)
]=])

set(HUNTER_PACKAGES iwasm)
set(HUNTER_FILEPATH_CONFIG ${CMAKE_BINARY_DIR}/config.cmake)

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

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(Boost_NO_BOOST_CMAKE ON)

find_package(iwasm CONFIG REQUIRED)

add_executable(wasmrunner main.cpp)
target_link_libraries(wasmrunner PRIVATE iwasm::vmlib)

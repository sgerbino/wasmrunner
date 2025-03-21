add_executable(wasmrunner main.cpp)
target_link_libraries(wasmrunner PRIVATE vmlib ${llvm_libs})

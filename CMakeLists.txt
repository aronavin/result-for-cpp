cmake_minimum_required(VERSION 3.10)

project(result_for_cpp CXX)

enable_testing()

add_library(result INTERFACE)

target_include_directories(result INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_subdirectory(samples)

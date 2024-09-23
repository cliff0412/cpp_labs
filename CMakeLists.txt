cmake_minimum_required(VERSION 3.29)
project(bench_string)
set(CMAKE_CXX_STANDARD 17) 

find_package(benchmark REQUIRED)

add_executable(bench_lists benches/bench_lists.cpp)
target_link_libraries(bench_lists benchmark::benchmark)


add_executable(lists lists/lists.cpp)

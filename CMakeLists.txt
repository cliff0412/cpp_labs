cmake_minimum_required(VERSION 3.29)
project(bench_string)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(USE_AVX "Enable AVX acceleration" OFF)
option(BUILD_BENCH "build bench" ON)

include(CheckCXXCompilerFlag)

# Check for AVX support
check_cxx_compiler_flag("-mavx" COMPILER_SUPPORTS_AVX)
if(COMPILER_SUPPORTS_AVX)
    message(STATUS "AVX support found, adding -mavx flag")
    add_compile_options(-mavx)
else()
    message(WARNING "AVX not supported")
endif()

# Check for AVX2 support (optional)
check_cxx_compiler_flag("-mavx2" COMPILER_SUPPORTS_AVX2)
if(COMPILER_SUPPORTS_AVX2)
    message(STATUS "AVX2 support found, adding -mavx2 flag")
    add_compile_options(-mavx2)
endif()

find_package(benchmark REQUIRED)
find_package(OpenMP)

if(BUILD_BENCH)
add_executable(bench_lists benches/bench_lists.cpp lists/lists.cpp lists/util.cpp)
# target_link_libraries(bench_lists PAPI::PAPI)
target_include_directories(bench_lists PRIVATE /usr/include/)
target_include_directories(bench_lists PRIVATE /usr/local/include/)
target_include_directories(bench_lists PRIVATE ${CMAKE_SOURCE_DIR}/unroll_list/)
target_include_directories(bench_lists PRIVATE ${CMAKE_SOURCE_DIR}/lists/)
target_link_libraries(bench_lists /usr/local/lib/libpapi.so)
target_link_libraries(bench_lists benchmark::benchmark)
if(OpenMP_CXX_FOUND)
    target_link_libraries(bench_lists OpenMP::OpenMP_CXX)
endif()

if(USE_AVX)
    message("USE_AVX=: ${USE_AVX}")
    target_compile_definitions(bench_lists PRIVATE __USE_AVX__ __NO_GMP__)
    target_compile_options(bench_lists PRIVATE -mavx2)
endif()
endif()

# add_executable(lists lists/lists.cpp)
# target_include_directories(lists PRIVATE ${CMAKE_SOURCE_DIR}/unroll_list/)
# if(USE_AVX)
#     message("USE_AVX=: ${USE_AVX}")
#     target_compile_definitions(lists PRIVATE __USE_AVX__ __NO_GMP__)
#     target_compile_options(lists PRIVATE -mavx2)
# endif()

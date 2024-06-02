cmake_minimum_required(VERSION 3.22)
project(skip_list)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")

# 引入 doctest 依赖
include(FetchContent)

FetchContent_Declare(
        doctest
        GIT_REPOSITORY https://github.com/doctest/doctest
)
FetchContent_MakeAvailable(doctest)

# 单元测试
add_executable(unit_test test.cpp skip_list.cpp)
target_link_libraries(unit_test doctest)

enable_testing()
add_test(NAME UnitTest COMMAND unit_test)

# 性能测试
add_executable(benchmark benchmark.cpp skip_list.cpp)
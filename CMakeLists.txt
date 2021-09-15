cmake_minimum_required (VERSION 3.16)

project(threadpool-example
        DESCRIPTION "C++11 threadpool example"
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    SET(PLATFORM_SPECIFIC_LIBS -lpthread)
endif()

add_library(threadpool_example INTERFACE)
target_include_directories(threadpool_example INTERFACE include)

add_executable(threadpool_demo src/threadpool_example/demo.cpp)
target_link_libraries(threadpool_demo PUBLIC threadpool_example)

find_package(Boost 1.65.0 REQUIRED COMPONENTS program_options)

set(COMPILE_OPTIONS -Wall -Werror -Wpedantic)
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    list(APPEND COMPILE_OPTIONS -O3 -flto)
endif()

target_compile_options(threadpool_demo PUBLIC ${COMPILE_OPTIONS})
target_include_directories(threadpool_demo PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(threadpool_demo PRIVATE ${Boost_LIBRARIES} ${PLATFORM_SPECIFIC_LIBS})

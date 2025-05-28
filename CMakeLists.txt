cmake_minimum_required(VERSION 3.12)
project(NConfig)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE TESTS tests/*.cpp)

add_executable(NConfigTests ${TESTS})
target_include_directories(NConfigTests PRIVATE
    include
    vendor/NCLI/include
    vendor/NTest/include
)
cmake_minimum_required(VERSION 3.10.0)
project(Parallel-Mergesort VERSION 0.1.0 LANGUAGES C CXX)

file(GLOB_RECURSE src CONFIGURE_DEPENDS
    "src/algorithms/*.cpp"
    "src/server/*.cpp"
    "src/client/*.cpp"
    "main.cpp"
)

add_executable(${PROJECT_NAME} ${src})

list(APPEND includePath 
    "include/algorithms"
    "include/client"
    "include/server"
)

target_include_directories(${PROJECT_NAME} PUBLIC ${includePath})

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})

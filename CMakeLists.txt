cmake_minimum_required(VERSION 3.10.0)
project(Parallel-Mergesort VERSION 0.1.0 LANGUAGES C CXX)

file(GLOB_RECURSE ALG_SRC CONFIGURE_DEPENDS
    src/algorithms/*.cpp
)

file(GLOB_RECURSE SERVER_SRC CONFIGURE_DEPENDS
    src/server/*.cpp
)

file(GLOB_RECURSE CLIENT_SRC CONFIGURE_DEPENDS
    src/client/*.cpp
)

add_executable(server
    src/server/main.cpp
    ${SERVER_SRC}
    ${ALG_SRC}
)

add_executable(client
    src/client/main.cpp
    ${CLIENT_SRC}
)

target_include_directories(server PUBLIC include)
target_include_directories(client PUBLIC include)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})
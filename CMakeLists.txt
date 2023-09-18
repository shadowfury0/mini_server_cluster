cmake_minimum_required(VERSION 3.2)

project(mini_ftp)
#添加线程库
find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_BUILD_TYPE "Debug")

# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

include_directories(${PROJECT_SOURCE_DIR}/include)
# aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC_LIST)
file(GLOB_RECURSE SRC_FILE "src/*.cpp" "src/*.c")

# link_directories(${PROJECT_SOURCE_DIR}/lib)

# add_library(clc STATIC ${SRC_LIST})
# add_library(clac SHARED ${SRC_LIST})

add_executable(server ${SRC_FILE} ${PROJECT_SOURCE_DIR}/test/server.cpp)
target_link_libraries(server Threads::Threads)

add_executable(client ${SRC_FILE} ${PROJECT_SOURCE_DIR}/test/client.cpp)
target_link_libraries(client Threads::Threads)

add_executable(test2 ${SRC_FILE} ${PROJECT_SOURCE_DIR}/test/test2.cpp)
target_link_libraries(test2 Threads::Threads)

# add_executable(test3 ${SRC_FILE} ${PROJECT_SOURCE_DIR}/test/test3.cpp)
# target_link_libraries(test3 Threads::Threads)

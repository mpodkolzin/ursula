cmake_minimum_required(VERSION 3.10.0)
project(bifrost VERSION 0.1.0 LANGUAGES C CXX)

#add_library(MyLib STATIC
#    foo.cpp
#)

#target_include_directories(bifrost PUBLIC
#    ${PROJECT_SOURCE_DIR}/include
#)

add_subdirectory(server)
add_subdirectory(io)
add_subdirectory(partition)
add_subdirectory(record)
add_subdirectory(broker)
add_subdirectory(metrics)
add_subdirectory(util)
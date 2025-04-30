
add_library(partition STATIC 
    partition_writer.cpp
    )

target_include_directories(partition PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_library(partition STATIC 
    partition_writer.cpp
    partition_manager.cpp
    partition_reader.cpp
    )

target_include_directories(partition PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(partition PRIVATE spdlog::spdlog)
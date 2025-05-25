add_library(io STATIC 
    file_handle.cpp
    buffered_writer.cpp
    record_reader.cpp
    )


target_include_directories(io PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(io PRIVATE spdlog::spdlog)
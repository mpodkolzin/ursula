
add_library(offset_store STATIC 
    file_offset_store.cpp
    )

target_include_directories(offset_store PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(offset_store PRIVATE spdlog::spdlog)
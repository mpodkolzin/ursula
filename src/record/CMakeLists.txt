
add_library(record STATIC 
    record.cpp
    )

target_include_directories(record PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(record PUBLIC spdlog::spdlog)

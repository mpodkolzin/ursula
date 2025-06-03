add_library(producer STATIC 
    partition_worker.cpp
    )

target_include_directories(producer PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(producer PRIVATE spdlog::spdlog)
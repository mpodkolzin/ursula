add_library(consumer STATIC 
    consumer.cpp
    consumer_group.cpp
    consumer_manager.cpp
    )

target_include_directories(consumer PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(consumer PRIVATE spdlog::spdlog)
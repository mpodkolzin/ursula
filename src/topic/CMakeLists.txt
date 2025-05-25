
add_library(topic STATIC 
    topic.cpp
    topic_manager.cpp
    )

target_include_directories(topic PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(topic PRIVATE spdlog::spdlog)
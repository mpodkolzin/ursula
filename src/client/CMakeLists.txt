add_library(broker_client STATIC 
    local_client.cpp
    )

target_include_directories(broker_client PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(broker_client PRIVATE spdlog::spdlog)
add_library(broker STATIC 
    broker.cpp
    )

target_include_directories(broker PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(broker PRIVATE spdlog::spdlog)

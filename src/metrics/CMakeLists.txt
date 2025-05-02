add_library(metrics STATIC 
    metrics_collector.cpp
    )

target_include_directories(metrics PUBLIC ${CMAKE_SOURCE_DIR}/include)
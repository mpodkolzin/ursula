add_library(io STATIC 
    file_handle.cpp
    )

target_include_directories(io PUBLIC ${CMAKE_SOURCE_DIR}/include)
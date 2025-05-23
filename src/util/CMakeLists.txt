add_library(util STATIC 
    crc32.cpp
    )

target_include_directories(util PUBLIC ${CMAKE_SOURCE_DIR}/include)

find_package(ZLIB REQUIRED)

target_link_libraries(util PRIVATE ZLIB::ZLIB)

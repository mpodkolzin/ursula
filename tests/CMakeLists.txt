include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.2 # or latest stable
)

FetchContent_MakeAvailable(Catch2)

add_executable(test_broker test_broker.cpp test_broker2.cpp)
#add_executable(test_broker2 test_broker2.cpp)

target_include_directories(test_broker PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(test_broker PRIVATE Catch2::Catch2WithMain)
target_link_libraries(test_broker PRIVATE spdlog::spdlog)
target_link_libraries(test_broker PRIVATE metrics)
target_link_libraries(test_broker PRIVATE partition)
target_link_libraries(test_broker PRIVATE record)
target_link_libraries(test_broker PRIVATE topic)
target_link_libraries(test_broker PRIVATE record)
target_link_libraries(test_broker PRIVATE util)
target_link_libraries(test_broker PRIVATE io)
target_link_libraries(test_broker PRIVATE consumer)
target_link_libraries(test_broker PRIVATE broker)
target_link_libraries(test_broker PRIVATE broker_client)
target_link_libraries(test_broker PRIVATE producer)
target_link_libraries(test_broker PRIVATE offset_store)
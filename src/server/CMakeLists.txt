
include(FetchContent)

FetchContent_Declare(
  httplib
  GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
  GIT_TAG v0.15.3  # Replace with desired tag/version
)

# Fetch spdlog
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.13.0 # or latest stable
)


include(FetchContent)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3 # or latest release tag
)



FetchContent_MakeAvailable(httplib)
FetchContent_MakeAvailable(spdlog)
FetchContent_MakeAvailable(json)

add_executable(ursula
    main.cpp
)

# Link the 'wal' library (must be defined before in the build order)
#target_link_libraries(main_exec PRIVATE wal)
#target_link_libraries(main_exec PRIVATE io)
target_link_libraries(ursula PRIVATE spdlog::spdlog)
target_link_libraries(ursula PRIVATE record)
target_link_libraries(ursula PRIVATE util)
target_link_libraries(ursula PRIVATE metrics)
target_link_libraries(ursula PRIVATE partition)
target_link_libraries(ursula PRIVATE io)
target_link_libraries(ursula PRIVATE broker)
target_link_libraries(ursula PRIVATE topic)
target_link_libraries(ursula PRIVATE consumer)
target_link_libraries(ursula PRIVATE httplib::httplib)
target_link_libraries(ursula PRIVATE nlohmann_json)
target_link_libraries(ursula PRIVATE broker_client)
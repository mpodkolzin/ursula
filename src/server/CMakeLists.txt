
include(FetchContent)

FetchContent_Declare(
  httplib
  GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
  GIT_TAG v0.15.3  # Replace with desired tag/version
)

FetchContent_MakeAvailable(httplib)


add_executable(ursula
    main.cpp
)

# Link the 'wal' library (must be defined before in the build order)
#target_link_libraries(main_exec PRIVATE wal)
#target_link_libraries(main_exec PRIVATE io)
target_link_libraries(ursula PRIVATE httplib::httplib)
target_link_libraries(ursula PRIVATE partition)
cmake_minimum_required(VERSION 3.16)
project(bifrost VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optional: Set policy explicitly if CMake is recent
cmake_policy(SET CMP0135 NEW)

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ----------------------
# Dependencies
# ----------------------
find_package(gRPC REQUIRED)
find_package(Protobuf REQUIRED MODULE) # only loads protoc executable, not targets

include_directories(${CMAKE_CURRENT_BINARY_DIR})  # for generated headers
include_directories(${Protobuf_INCLUDE_DIRS})


# ----------------------
# Macro: Generate Protobuf and gRPC code
# ----------------------
function(generate_proto_grpc OUT_SRCS OUT_HDRS PROTO_FILES PROTO_DIR)
  set(_srcs "")
  set(_hdrs "")
  message("generate_proto_grpc:: Generating Protobuf from ${PROTO_FILES}")

  foreach(proto_file ${PROTO_FILES})
    get_filename_component(proto_name ${proto_file} NAME_WE)
    set(proto_src "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.pb.cc")
    message("Generating Protobuf from ${proto_src}")
    set(proto_hdr "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.pb.h")
    set(grpc_src "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.grpc.pb.cc")
    set(grpc_hdr "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.grpc.pb.h")

    add_custom_command(
      OUTPUT ${proto_src} ${proto_hdr}
      COMMAND protobuf::protoc
      ARGS --proto_path=${PROTO_DIR}
           --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
           ${proto_file}
      DEPENDS ${proto_file}
      COMMENT "Generating Protobuf from ${proto_file}"
      VERBATIM
    )

    add_custom_command(
      OUTPUT ${grpc_src} ${grpc_hdr}
      COMMAND protobuf::protoc
      ARGS --proto_path=${PROTO_DIR}
           --grpc_out=${CMAKE_CURRENT_BINARY_DIR}
           --plugin=protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>
           ${proto_file}
      DEPENDS ${proto_file}
      COMMENT "Generating gRPC from ${proto_file}"
      VERBATIM
    )

    list(APPEND _srcs ${proto_src} ${grpc_src})
    list(APPEND _hdrs ${proto_hdr} ${grpc_hdr})
  endforeach()

  set(${OUT_SRCS} ${_srcs} PARENT_SCOPE)
  set(${OUT_HDRS} ${_hdrs} PARENT_SCOPE)
endfunction()

# ----------------------
# Protobuf + gRPC Codegen for your .proto files
# ----------------------
file(GLOB PROTO_FILES "${CMAKE_SOURCE_DIR}/proto/*.proto")
message("PROTO_FILES: ${PROTO_FILES}")  

generate_proto_grpc(PROTO_SRCS PROTO_HDRS "${PROTO_FILES}" "${CMAKE_SOURCE_DIR}/proto")

add_library(proto_generated ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(proto_generated
  PUBLIC
    gRPC::grpc++
    protobuf::libprotobuf
)

# ----------------------
# Your Code
# ----------------------
add_subdirectory(src)
add_subdirectory(tests)

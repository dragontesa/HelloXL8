# Copyright 2018 gRPC authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# cmake build file for C++ route_guide example.
# Assumes protobuf and gRPC have been installed using cmake.
# See cmake_externalproject/CMakeLists.txt for all-in-one cmake build
# that automatically builds all the dependencies before building route_guide.

cmake_minimum_required(VERSION 3.8)

project(e2e_pipe_api C CXX)

include(../cmake/grpc.cmake)

# Proto file
get_filename_component(e2e_pipe_proto "e2e_pipe/api/e2e_api_layer_proto.proto" ABSOLUTE)
get_filename_component(e2e_pipe_proto_path "${e2e_pipe_proto}" PATH)
get_filename_component(google_proto "google/api/annotations.proto" ABSOLUTE)
get_filename_component(google_proto_path "${google_proto}" PATH)

# Generated sources
set(e2e_pipe_proto_h "${CMAKE_CURRENT_BINARY_DIR}/e2e_api_layer_proto.pb.h")
set(e2e_pipe_proto_cc "${CMAKE_CURRENT_BINARY_DIR}/e2e_api_layer_proto.pb.cc")
set(e2e_pipe_grpc_h "${CMAKE_CURRENT_BINARY_DIR}/e2e_api_layer_proto.grpc.pb.h")
set(e2e_pipe_grpc_cc "${CMAKE_CURRENT_BINARY_DIR}/e2e_api_layer_proto.grpc.pb.cc")
#set(e2e_pipe_proto_api_h "${CMAKE_CURRENT_SOURCE_DIR}/e2e_pipe/api/e2e_api_layer_proto.pb.h")
#set(e2e_pipe_grpc_api_h "${CMAKE_CURRENT_SOURCE_DIR}/e2e_pipe/api/e2e_api_layer_proto.grpc.pb.h")

add_custom_command(
      OUTPUT "${e2e_pipe_proto_h}" "${e2e_pipe_proto_cc}" "${e2e_pipe_grpc_h}" "${e2e_pipe_grpc_cc}" 
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${e2e_pipe_proto_path}"
        --proto_path="${CMAKE_CURRENT_SOURCE_DIR}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${e2e_pipe_proto}"
      DEPENDS "${e2e_pipe_proto}")

# Include generated *.pb.h files
#include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# e2e_pipe_proto_api
add_library(e2e_pipe_api
  ${e2e_pipe_proto_h}
  ${e2e_pipe_grpc_h})

target_link_libraries(e2e_pipe_api
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})


# post build copy build-out to api location
function(copy_e2e_api_headers target targetItem)
add_custom_command(
        TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            "$<TARGET_PROPERTY:${target},BINARY_DIR>/${targetItem}"
            "$<TARGET_PROPERTY:${target},SOURCE_DIR>/e2e_pipe/api/${targetItem}"
    )
endfunction()

copy_e2e_api_headers(${PROJECT_NAME} e2e_api_layer_proto.pb.h)
copy_e2e_api_headers(${PROJECT_NAME} e2e_api_layer_proto.grpc.pb.h)
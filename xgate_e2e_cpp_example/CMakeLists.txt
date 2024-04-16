cmake_minimum_required(VERSION 3.22)
project(XL8Example VERSION 0.1.0)
# set build type
# set(CMAKE_BUILD_TYPE "RelWithDebInfo")
# SET(CMAKE_CONFIGURATION_TYPES "Debug") # this variable will be specified at configure time 
# set (CMAKE_CXX_STANDARD 14)
# set(CMAKE_CXX_FLAGS "/MTd") # This cause LNK2038	'MTd_StaticDebug'  MD_DynamicRelease'
include(cmake/grpc.cmake)

# Proto file
get_filename_component(protos_path "protos" ABSOLUTE)
get_filename_component(e2e_pipe_proto "protos/e2e_pipe/api/e2e_api_layer_proto.proto" ABSOLUTE)
get_filename_component(e2e_pipe_proto_path "${e2e_pipe_proto}" PATH)
get_filename_component(google_annotation_proto "protos/google/api/annotations.proto" ABSOLUTE)
get_filename_component(google_annotation_proto_path "${google_annotation_proto}" PATH)
get_filename_component(google_http_proto "protos/google/api/http.proto" ABSOLUTE)
get_filename_component(google_http_proto_path "${google_http_proto}" PATH)

# Generated sources
set(e2e_pipe_proto_h "${CMAKE_CURRENT_BINARY_DIR}/e2e_pipe/api/e2e_api_layer_proto.pb.h")
set(e2e_pipe_proto_cc "${CMAKE_CURRENT_BINARY_DIR}/e2e_pipe/api/e2e_api_layer_proto.pb.cc")
set(e2e_pipe_grpc_h "${CMAKE_CURRENT_BINARY_DIR}/e2e_pipe/api/e2e_api_layer_proto.grpc.pb.h")
set(e2e_pipe_grpc_cc "${CMAKE_CURRENT_BINARY_DIR}/e2e_pipe/api/e2e_api_layer_proto.grpc.pb.cc")

set(google_annotation_proto_h "${CMAKE_CURRENT_BINARY_DIR}/google/api/annotations.pb.h")
set(google_annotation_proto_cc "${CMAKE_CURRENT_BINARY_DIR}/google/api/annotations.pb.cc")
set(google_annotation_grpc_h "${CMAKE_CURRENT_BINARY_DIR}/google/api/annotations.grpc.pb.h")
set(google_annotation_grpc_cc "${CMAKE_CURRENT_BINARY_DIR}/google/api/annotations.grpc.pb.cc")

set(google_http_proto_h "${CMAKE_CURRENT_BINARY_DIR}/google/api/http.pb.h")
set(google_http_proto_cc "${CMAKE_CURRENT_BINARY_DIR}/google/api/http.pb.cc")
set(google_http_grpc_h "${CMAKE_CURRENT_BINARY_DIR}/google/api/http.grpc.pb.h")
set(google_http_grpc_cc "${CMAKE_CURRENT_BINARY_DIR}/google/api/http.grpc.pb.cc")

add_custom_command(
    OUTPUT "${e2e_pipe_proto_h}" "${e2e_pipe_proto_cc}" "${e2e_pipe_grpc_h}" "${e2e_pipe_grpc_cc}"
    COMMAND ${CMAKE_COMMAND} -E env "LD_LIBRARY_PATH=$ENV{LD_LIBRARY_PATH}:${GRPC_SDK_DIR}/lib:${GRPC_SDK_DIR}/lib64"
    ${_PROTOBUF_PROTOC}
    ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
      --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
      -I "${protos_path}"
      --proto_path="${CMAKE_CURRENT_SOURCE_DIR}/protos"
      --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
      "${e2e_pipe_proto}"
    DEPENDS "${e2e_pipe_proto}")


add_custom_command(
  OUTPUT "${google_annotation_proto_h}" "${google_annotation_proto_cc}"  "${google_annotation_grpc_h}" "${google_annotation_grpc_cc}"
  COMMAND ${CMAKE_COMMAND} -E env "LD_LIBRARY_PATH=$ENV{LD_LIBRARY_PATH}:${GRPC_SDK_DIR}/lib:${GRPC_SDK_DIR}/lib64"
  ${_PROTOBUF_PROTOC}
  ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
    --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
    -I "${protos_path}"
    --proto_path="${CMAKE_CURRENT_SOURCE_DIR}/protos"
    --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
    "${google_annotation_proto}"
  DEPENDS "${google_annotation_proto}")


add_custom_command(
  OUTPUT "${google_http_proto_h}" "${google_http_proto_cc}" "${google_http_grpc_h}" "${google_http_grpc_cc}"
  COMMAND ${CMAKE_COMMAND} -E env "LD_LIBRARY_PATH=$ENV{LD_LIBRARY_PATH}:${GRPC_SDK_DIR}/lib:${GRPC_SDK_DIR}/lib64"
  ${_PROTOBUF_PROTOC}
  ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
    --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
    -I "${protos_path}"
    --proto_path="${CMAKE_CURRENT_SOURCE_DIR}/protos"
    --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
    "${google_http_proto}"
  DEPENDS "${google_http_proto}")
        
  # Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# google protos api
add_library(google_http_api
 ${google_http_proto_h}
 ${google_http_proto_cc}
 ${google_http_grpc_h}
 ${google_http_grpc_cc}
)

target_include_directories(google_http_api PRIVATE
 ${PROJECT_SOURCE_DIR}/protos 
 ${GRPC_SDK_DIR}/include
)

target_link_libraries(google_http_api 
${_REFLECTION}
${_GRPC_GRPCPP}
${_PROTOBUF_LIBPROTOBUF}
)

add_library(google_api 
 ${google_annotation_proto_h}
 ${google_annotation_proto_cc}
 ${google_annotation_grpc_h}
 ${google_annotation_grpc_cc}
)

target_include_directories(google_api PRIVATE
 ${PROJECT_SOURCE_DIR}/protos 
 ${GRPC_SDK_DIR}/include
)

target_link_directories(google_api PRIVATE 
  ${GRPC_SDK_DIR}/lib
  ${GRPC_SDK_DIR}/lib64
  ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(google_api 
 google_http_api
 ${_REFLECTION}
 ${_GRPC_GRPCPP}
 ${_PROTOBUF_LIBPROTOBUF})

# e2e_pipe protos api
add_library(e2e_pipe_api 
  ${e2e_pipe_proto_h}
  ${e2e_pipe_proto_cc}
  ${e2e_pipe_grpc_h}
  ${e2e_pipe_grpc_cc}
  )

target_include_directories(e2e_pipe_api PRIVATE
  ${PROJECT_SOURCE_DIR}/protos 
  ${GRPC_SDK_DIR}/include
)

target_link_libraries(e2e_pipe_api 
google_http_api
google_api
${_REFLECTION}
${_GRPC_GRPCPP}
${_PROTOBUF_LIBPROTOBUF}
)

# message("user home is "$ENV{HOME})

# copy api to project api source
function(copy_protos_api target item assembly)
    add_custom_command(
        TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            "$<TARGET_PROPERTY:${target},BINARY_DIR>/${item}"
            "$<TARGET_PROPERTY:${target},SOURCE_DIR>/protos/${assembly}/api/${item}")
endfunction()

#copy_protos_api(google_api http.pb.h "google")
#copy_protos_api(google_api http.grpc.pb.h "google")
#copy_protos_api(google_api annotations.pb.h "google")
#copy_protos_api(google_api annotations.grpc.pb.h "google")
#copy_protos_api(e2e_pipe_api e2e_api_layer_proto.pb.h "e2e_pipe")
#copy_protos_api(e2e_pipe_api e2e_api_layer_proto.grpc.pb.h "e2e_pipe")
#copy_protos_api(e2e_pipe_api libe2e_pipe_api.a)

# build example target
add_executable(${PROJECT_NAME} wave_to_text.cpp)
# target_compile_definitions(${PROJECT_NAME} PRIVATE "DEBUG=$<IF:$<CONFIG:Debug>,1,0>")
# add_compile_definitions("DEBUG=$<CONFIG:Debug>")
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_14)
target_include_directories(${PROJECT_NAME} PRIVATE
  ${PROJECT_SOURCE_DIR}/protos 
  ${GRPC_SDK_DIR}/include
  )

target_link_directories(${PROJECT_NAME} PRIVATE 
${GRPC_SDK_DIR}/lib
${GRPC_SDK_DIR}/lib64
)

message("cmake config type:")
# if($<CONFIG:Debug>)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  # if (CMAKE_CONFIGURATION_TYPES STREQUAL "Debug")
   message("build type is debug")
else()
   message("build type is release")
endif()

if(UNIX)
  target_link_libraries(${PROJECT_NAME} pthread dl grpc++_reflection protobuf grpc++ e2e_pipe_api)
elseif(WIN32)
   if(CMAKE_BUILD_TYPE STREQUAL "Debug")
     target_link_libraries(${PROJECT_NAME} grpc++_reflection libprotobufd grpc++ e2e_pipe_api)
   else()
     target_link_libraries(${PROJECT_NAME} grpc++_reflection libprotobuf grpc++ e2e_pipe_api)
   endif()
endif()

#enable_testing()
#include(CTest)
#set(CPACK_PROJECT_NAME ${PROJECT_NAME})
#set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
#include(CPack)
# post build copy build-out to api location

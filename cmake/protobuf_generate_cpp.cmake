# generate c++ source/header
get_filename_component(PROTO_FILE ${PROTO_FILE} ABSOLUTE)
get_filename_component(PROTO_PATH ${PROTO_FILE} PATH)

get_filename_component(_proto_we ${PROTO_FILE} NAME_WE)
set(_proto_source "${_proto_we}.pb.cc")
set(_proto_header "${_proto_we}.pb.h")

execute_process(
  COMMAND ${Protobuf_PROTOC_EXECUTABLE}
    --cpp_out dllexport_decl=BCP_API:${CMAKE_CURRENT_SOURCE_DIR}
    --proto_path ${PROTO_PATH}
    ${PROTO_FILE}
  RESULT_VARIABLE _result )

if (NOT _result EQUAL 0)
  message(FATAL_ERROR "Error processing ${PROTO_FILE} (${_result})")
  return()
endif()

# modify generated header to introduce define.hpp inclusion
file(READ ${_proto_header} _proto_header_contents)

string(REPLACE
  "// @@protoc_insertion_point(includes)"
  "#include <bitcoin/protocol/define.hpp>\n// @@protoc_insertion_point(includes)"
  _proto_header_contents "${_proto_header_contents}")

file(WRITE ${_proto_header} "${_proto_header_contents}")

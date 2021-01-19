INCLUDE(FindProtobuf)

FIND_PACKAGE(Protobuf REQUIRED)

INCLUDE_DIRECTORIES(${PROTOBUF_INCLUDE_DIR})

PROTOBUF_GENERATE_CPP(PROTO_SRC PROTO_HEADER message.proto)

ADD_LIBRARY(chat_server_proto ${PROTO_HEADER} ${PROTO_SRC})

target_link_libraries(chat_server_proto PRIVATE ${PROTOBUF_LIBRARY})

set(ProtobufIncludePath ${CMAKE_CURRENT_BINARY_DIR}
        CACHE INTERNAL "Path to generated protobuf files.")
add_library(proto
    ProtobufCodec.h
    ProtobufDispatcher.h
    ProtobufCodec.cpp
    ProtobufDispatcher.cpp)

protobuf_generate(
    PROTOC_OUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}
    TARGET proto
    PROTOS ${CMAKE_CURRENT_SOURCE_DIR}/BikeSharing.proto)

target_link_libraries(proto protobuf::libprotobuf ${MUDUO_BASE} ${MUDUO_NET})
target_include_directories(proto PUBLIC ${CMAKE_SOURCE_DIR})
target_include_directories(proto INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(test)

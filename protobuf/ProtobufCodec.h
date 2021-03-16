#ifndef PROTOBUF_CODEC_H
#define PROTOBUF_CODEC_H

#include <cstddef>
#include <functional>
#include <utility>

#include <muduo/net/TcpConnection.h>

#include "Messages.h"
#include "int.h"

// struct ProtobufTransportFormat {
//     int32 len;

//     int32 nameLen;
//     char  typeName[nameLen];
//     char  protobufData[len - nameLen - 8];
// };

class ProtobufCodec {
public:
    enum class ErrorCode {
        NoError,
        InvalidLength,
        InvalidNameLen,
        UnknownMessageType,
        ParseError
    };

    using ProtobufMessageCallback = std::function<void(const muduo::net::TcpConnectionPtr&,
                                                       const MessagePtr&,
                                                       muduo::Timestamp)>;

    using ErrorCallback = std::function<void(const muduo::net::TcpConnectionPtr&,
                                             muduo::net::Buffer*,
                                             muduo::Timestamp,
                                             ErrorCode)>;

    ProtobufCodec(ProtobufMessageCallback messageCb);

    void onMessage(const muduo::net::TcpConnectionPtr& conn,
                   muduo::net::Buffer* buf,
                   muduo::Timestamp receiveTime);

    static void send(const muduo::net::TcpConnectionPtr& conn, const google::protobuf::Message& message);
    static void fillEmptyBuffer(muduo::net::Buffer* buf, const google::protobuf::Message& message);
    static google::protobuf::Message* createMessage(const std::string& typeName);
    static MessagePtr parse(const char* buf, std::size_t len, ErrorCode& errorCode);

private:
    constexpr static auto HeaderLen = sizeof(int32);
    constexpr static auto MaxMessageLen = 64 * 1024 * 1024;

    static void defaultErrorCallback(const muduo::net::TcpConnectionPtr&,
                                     muduo::net::Buffer*,
                                     muduo::Timestamp,
                                     ErrorCode);

    ProtobufMessageCallback messageCallback;
    ErrorCallback errorCallback;
};

#endif

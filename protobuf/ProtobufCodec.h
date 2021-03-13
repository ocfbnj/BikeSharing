#ifndef PROTOBUF_CODEC_H
#define PROTOBUF_CODEC_H

#include <cstddef>
#include <functional>
#include <utility>

#include <muduo/net/Buffer.h>
#include <muduo/net/TcpConnection.h>

#include "Messages.h"
#include "int.h"

// struct ProtobufTransportFormat {
//     int32 len;
//
//     int32 nameLen;
//     char typeName[nameLen];
//     char protobufData[len - nameLen - 8];
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

    using ProtobufMessageCallback = std::function<void(muduo::net::TcpConnectionPtr,
                                                       MessagePtr,
                                                       muduo::Timestamp)>;
    using ErrorCallback = std::function<void(muduo::net::TcpConnectionPtr,
                                             muduo::net::Buffer*,
                                             muduo::Timestamp,
                                             ErrorCode)>;

    ProtobufCodec(ProtobufMessageCallback messageCb,
                  ErrorCallback errorCb = &ProtobufCodec::defaultErrorCallback_)
        : messageCallback_(std::move(messageCb)), errorCallback_(errorCb) {}

    void onMessage(const muduo::net::TcpConnectionPtr& conn,
                   muduo::net::Buffer* buf,
                   muduo::Timestamp receiveTime);

    void send(muduo::net::TcpConnectionPtr conn, const google::protobuf::Message& message);

    static void fillEmptyBuffer(muduo::net::Buffer* buf, const google::protobuf::Message& message);
    static google::protobuf::Message* createMessage(const std::string& typeName);
    static MessagePtr parse(const char* buf, std::size_t len, ErrorCode& errorCode);

private:
    constexpr static auto HeaderLen_ = sizeof(int32);
    constexpr static auto MaxMessageLen_ = 64 * 1024 * 1024;

    static void defaultErrorCallback_(muduo::net::TcpConnectionPtr,
                                      muduo::net::Buffer*,
                                      muduo::Timestamp,
                                      ErrorCode);

    ProtobufMessageCallback messageCallback_;
    ErrorCallback errorCallback_;
};

#endif

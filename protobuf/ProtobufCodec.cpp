#include <string_view>
#include <unordered_map>

#include <muduo/base/Logging.h>
#include <muduo/net/Endian.h>

#include "ProtobufCodec.h"

static std::unordered_map<ProtobufCodec::ErrorCode, std::string_view> errorMap = {
    {ProtobufCodec::ErrorCode::NoError, "No error"},
    {ProtobufCodec::ErrorCode::InvalidLength, "Invalid length"},
    {ProtobufCodec::ErrorCode::InvalidNameLen, "Invalid name length"},
    {ProtobufCodec::ErrorCode::UnknownMessageType, "Unknown message type"},
    {ProtobufCodec::ErrorCode::ParseError, "Parse error"},
};

static int32 asInt32(const char* buf) {
    int32 be32 = 0;
    ::memcpy(&be32, buf, sizeof(be32));
    return muduo::net::sockets::networkToHost32(be32);
}

void ProtobufCodec::send(muduo::net::TcpConnectionPtr conn, const google::protobuf::Message& message) {
    muduo::net::Buffer buffer;
    ProtobufCodec::fillEmptyBuffer(&buffer, message);
    conn->send(&buffer);
}

void ProtobufCodec::defaultErrorCallback_(muduo::net::TcpConnectionPtr conn,
                                          muduo::net::Buffer* buf,
                                          muduo::Timestamp receiveTime,
                                          ErrorCode errorCode) {
    LOG_ERROR << "ProtobufCodec::defaultErrorCallback_ - " << errorMap[errorCode].data();

    if (conn && conn->connected()) {
        conn->shutdown();
    }
}

void ProtobufCodec::onMessage(const muduo::net::TcpConnectionPtr& conn,
                              muduo::net::Buffer* buf,
                              muduo::Timestamp receiveTime) {
    while (buf->readableBytes() > HeaderLen_) {
        int32 len = buf->peekInt32();
        if (buf->readableBytes() >= HeaderLen_ + len) {
            ErrorCode err = ErrorCode::NoError;
            MessagePtr message = parse(buf->peek() + HeaderLen_, len, err);

            if (err == ErrorCode::NoError && message) {
                messageCallback_(std::move(conn), std::move(message), std::move(receiveTime));
                buf->retrieve(HeaderLen_ + len);
            } else {
                errorCallback_(std::move(conn), buf, std::move(receiveTime), err);
                break;
            }
        } else {
            break;
        }
    }
}

MessagePtr ProtobufCodec::parse(const char* buf, std::size_t len, ErrorCode& errorCode) {
    MessagePtr message;

    int32 nameLen = asInt32(buf);
    if (nameLen >= 2 && nameLen <= len - HeaderLen_) {
        std::string typeName{buf + HeaderLen_, static_cast<std::size_t>(nameLen - 1)};

        message.reset(ProtobufCodec::createMessage(typeName));
        if (message) {
            const char* data = buf + HeaderLen_ + nameLen;
            int32 dataLen = len - HeaderLen_ - nameLen;

            if (message->ParseFromArray(data, dataLen)) {
                errorCode = ErrorCode::NoError;
            } else {
                errorCode = ErrorCode::ParseError;
            }
        } else {
            errorCode = ErrorCode::UnknownMessageType;
        }

    } else {
        errorCode = ErrorCode::InvalidNameLen;
    }

    return message;
}

google::protobuf::Message* ProtobufCodec::createMessage(const std::string& typeName) {
    google::protobuf::Message* message = nullptr;

    auto ret = google::protobuf::DescriptorPool::generated_pool();

    const google::protobuf::Descriptor* descriptor =
        google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(typeName);
    if (descriptor) {
        const google::protobuf::Message* prototype =
            google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
        if (prototype) {
            message = prototype->New();
        }
    }

    return message;
}

void ProtobufCodec::fillEmptyBuffer(muduo::net::Buffer* buf, const google::protobuf::Message& message) {
    std::string typeName = message.GetTypeName();
    int32 nameLen = static_cast<int32>(typeName.size() + 1);

    buf->appendInt32(nameLen);
    buf->append(typeName.data(), nameLen);

    if (!message.IsInitialized()) {
        LOG_ERROR << "Serializing error: " << message.InitializationErrorString();
    }

    int byteSize = google::protobuf::internal::ToIntSize(message.ByteSizeLong());
    buf->ensureWritableBytes(byteSize);

    uint8_t* start = reinterpret_cast<uint8*>(buf->beginWrite());
    uint8_t* end = message.SerializeWithCachedSizesToArray(start);

    if (end - start != byteSize) {
        LOG_ERROR << "Serializing error: Byte size is not consistency";
    }
    buf->hasWritten(byteSize);

    int32 len = muduo::net::sockets::hostToNetwork32(static_cast<int32>(buf->readableBytes()));
    buf->prepend(&len, sizeof len);
}

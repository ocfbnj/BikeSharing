#include <iostream>

#include <gtest/gtest.h>

#include "ProtobufCodec.h"
#include "ProtobufDispatcher.h"

void print(const muduo::net::Buffer& buf) {
    printf("encoded to %zd bytes\n", buf.readableBytes());
    for (size_t i = 0; i < buf.readableBytes(); ++i) {
        unsigned char ch = static_cast<unsigned char>(buf.peek()[i]);

        printf("%2zd:  0x%02x  %c\n", i, ch, isgraph(ch) ? ch : ' ');
    }
}

GTEST_TEST(ProtobufCodec, MobileReqNoError) {
    BikeSharing::MobileReq req;
    req.set_mobile("12345678900");

    muduo::net::Buffer buf;
    ProtobufCodec::fillEmptyBuffer(&buf, req);
    print(buf);

    int32 len = buf.readInt32();
    ASSERT_TRUE(len == static_cast<int32>(buf.readableBytes()));

    ProtobufCodec::ErrorCode errorCode = ProtobufCodec::ErrorCode::NoError;
    MessagePtr message = ProtobufCodec::parse(buf.peek(), len, errorCode);
    ASSERT_TRUE(errorCode == ProtobufCodec::ErrorCode::NoError);
    ASSERT_TRUE(message != nullptr);

    message->PrintDebugString();

    ASSERT_TRUE(message->DebugString() == req.DebugString());
}

GTEST_TEST(ProtobufCodec, MobileReqParseError) {
    BikeSharing::MobileReq req;
    // req.set_mobile("12345678900");

    muduo::net::Buffer buf;
    ProtobufCodec::fillEmptyBuffer(&buf, req);
    print(buf);

    int32 len = buf.readInt32();
    ASSERT_TRUE(len == static_cast<int32>(buf.readableBytes()));

    ProtobufCodec::ErrorCode errorCode = ProtobufCodec::ErrorCode::NoError;
    MessagePtr message = ProtobufCodec::parse(buf.peek(), len, errorCode);
    ASSERT_TRUE(errorCode == ProtobufCodec::ErrorCode::ParseError);
}

void onMobileReq(const muduo::net::TcpConnectionPtr&,
                 const MobileReqPtr& message,
                 muduo::Timestamp) {
    std::cout << "onMobileReq: " << message->GetTypeName() << "\n";
}

void onUnknownMessageType(const muduo::net::TcpConnectionPtr&,
                          const MessagePtr& message,
                          muduo::Timestamp) {
    std::cout << "onUnknownMessageType: " << message->GetTypeName() << "\n";
}

GTEST_TEST(ProtobufDispatcher, Dispatch) {
    muduo::net::TcpConnectionPtr conn;
    muduo::Timestamp t;

    auto mobileReq = std::make_shared<BikeSharing::MobileReq>();
    auto mobileRsp = std::make_shared<BikeSharing::MobileRsp>();

    ProtobufDispatcher dispatcher{onUnknownMessageType};

    dispatcher.registerMessageCallback<BikeSharing::MobileReq>(onMobileReq);
    dispatcher.onProtobufMessage(conn, mobileReq, t);
    dispatcher.onProtobufMessage(conn, mobileRsp, t);
}

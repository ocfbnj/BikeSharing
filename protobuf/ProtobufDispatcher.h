#ifndef PROTOBUF_DISPATCHER_H
#define PROTOBUF_DISPATCHER_H

#include <memory>
#include <type_traits>
#include <unordered_map>

#include <google/protobuf/message.h>
#include <muduo/net/TcpConnection.h>

#include "Messages.h"

class Callback {
public:
    virtual ~Callback() = default;
    virtual void onMessage(muduo::net::TcpConnectionPtr,
                           MessagePtr,
                           muduo::Timestamp) const = 0;
};

template <typename T>
class CallbackT : public Callback {
    static_assert(std::is_base_of<google::protobuf::Message, T>::value,
                  "T must be derived from google::protobuf::Message.");

public:
    using ProtobufMessageTCallback = std::function<void(muduo::net::TcpConnectionPtr,
                                                        std::shared_ptr<T>,
                                                        muduo::Timestamp)>;

    CallbackT(ProtobufMessageTCallback callback)
        : callback_(std::move(callback)) {
    }

    void onMessage(muduo::net::TcpConnectionPtr conn,
                   MessagePtr message,
                   muduo::Timestamp receiveTime) const override {
        std::shared_ptr<T> concrete = muduo::down_pointer_cast<T>(message);
        assert(concrete != NULL);

        callback_(std::move(conn), std::move(concrete), std::move(receiveTime));
    }

private:
    ProtobufMessageTCallback callback_;
};

class ProtobufDispatcher {
public:
    using ProtobufMessageCallback = std::function<void(muduo::net::TcpConnectionPtr,
                                                       MessagePtr,
                                                       muduo::Timestamp)>;

    explicit ProtobufDispatcher(ProtobufMessageCallback defaultCb)
        : defaultCallback_(std::move(defaultCb)) {
    }

    void onProtobufMessage(muduo::net::TcpConnectionPtr conn,
                           MessagePtr message,
                           muduo::Timestamp receiveTime) const {
        auto it = callbacks_.find(message->GetDescriptor());
        if (it != callbacks_.end()) {
            it->second->onMessage(std::move(conn), std::move(message), std::move(receiveTime));
        } else {
            defaultCallback_(std::move(conn), std::move(message), std::move(receiveTime));
        }
    }

    template <typename T>
    void registerMessageCallback(typename CallbackT<T>::ProtobufMessageTCallback callback) {
        callbacks_[T::descriptor()] =
            std::make_shared<CallbackT<T>>(std::move(callback));
    }

private:
    using CallbackMap =
        std::unordered_map<const google::protobuf::Descriptor*, std::shared_ptr<Callback>>;

    CallbackMap callbacks_;
    ProtobufMessageCallback defaultCallback_;
};

#endif

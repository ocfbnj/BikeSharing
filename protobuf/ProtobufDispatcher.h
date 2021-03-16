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
    virtual void onMessage(const muduo::net::TcpConnectionPtr&,
                           const MessagePtr&,
                           muduo::Timestamp) const = 0;
};

template <typename T>
class CallbackT : public Callback {
    static_assert(std::is_base_of<google::protobuf::Message, T>::value,
                  "T must be derived from google::protobuf::Message.");

public:
    using ProtobufMessageTCallback = std::function<void(const muduo::net::TcpConnectionPtr&,
                                                        std::shared_ptr<T>,
                                                        muduo::Timestamp)>;

    CallbackT(ProtobufMessageTCallback callback) : callback(std::move(callback)) {}

    void onMessage(const muduo::net::TcpConnectionPtr& conn,
                   const MessagePtr& message,
                   muduo::Timestamp receiveTime) const override {
        std::shared_ptr<T> concrete = muduo::down_pointer_cast<T>(message);
        assert(concrete != NULL);

        callback(conn, concrete, receiveTime);
    }

private:
    ProtobufMessageTCallback callback;
};

class ProtobufDispatcher {
public:
    using ProtobufMessageCallback = std::function<void(const muduo::net::TcpConnectionPtr&,
                                                       const MessagePtr&,
                                                       muduo::Timestamp)>;

    explicit ProtobufDispatcher(ProtobufMessageCallback defaultCb)
        : defaultCallback(std::move(defaultCb)) {
    }

    void onProtobufMessage(const muduo::net::TcpConnectionPtr& conn,
                           const MessagePtr& message,
                           muduo::Timestamp receiveTime) const {
        auto it = callbacks.find(message->GetDescriptor());
        if (it != callbacks.end()) {
            it->second->onMessage(conn, message, receiveTime);
        } else {
            defaultCallback(conn, message, receiveTime);
        }
    }

    template <typename T>
    void registerMessageCallback(typename CallbackT<T>::ProtobufMessageTCallback callback) {
        callbacks[T::descriptor()] = std::make_shared<CallbackT<T>>(std::move(callback));
    }

private:
    using CallbackMap =
        std::unordered_map<const google::protobuf::Descriptor*, std::shared_ptr<Callback>>;

    CallbackMap callbacks;
    ProtobufMessageCallback defaultCallback;
};

#endif

#ifndef SERVICE_H
#define SERVICE_H

class ProtobufDispatcher;

class Service {
public:
    virtual ~Service() = default;

    virtual void registerInDispatcher(ProtobufDispatcher& dispatcher) = 0;
};

#endif

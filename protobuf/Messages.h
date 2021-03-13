#ifndef MESSAGES_H
#define MESSAGES_H

#include <memory>

#include <google/protobuf/message.h>

#include "BikeSharing.pb.h"

using MessagePtr = std::shared_ptr<google::protobuf::Message>;

using MobileReqPtr = std::shared_ptr<BikeSharing::MobileReq>;
using MobileRspPtr = std::shared_ptr<BikeSharing::MobileRsp>;
using LoginReqPtr = std::shared_ptr<BikeSharing::LoginReq>;
using LoginRspPtr = std::shared_ptr<BikeSharing::LoginRsp>;
using RechargeReqPtr = std::shared_ptr<BikeSharing::RechargeReq>;
using MobileRspPtr = std::shared_ptr<BikeSharing::MobileRsp>;

#endif

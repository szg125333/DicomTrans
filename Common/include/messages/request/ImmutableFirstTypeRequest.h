// src/messages/request/ImmutableFirstTypeRequest.h
#pragma once
#include "../../core/IMessage.h"
#include <string>

class ImmutableFirstTypeRequest : public IMessage {
public:
    // 类型名常量（协议标识）
    static constexpr const char* TYPE_NAME = "req_FirstType";
    ImmutableFirstTypeRequest(double gantryAngle, const std::string& speed = "Normal");

    std::string getTypeName() const override { return TYPE_NAME; }
    std::string toJson() const override;

    double getGantryAngle() const { return gantryAngle_; }
    std::string getSpeed() const { return speed_; }

private:
    double gantryAngle_;
    std::string speed_;
};
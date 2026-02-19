#pragma once
#include "IMessage.h"
#include <json/json.h>
#include <memory>

class IMessageBuilder {
public:
    virtual ~IMessageBuilder() = default;
    virtual std::unique_ptr<IMessage> build(const Json::Value& json) const = 0;
};
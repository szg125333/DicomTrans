#pragma once
#include "IMessage.h"
#ifdef _WIN32
#include <json/json.h>
#else
#include <jsoncpp/json/json.h>
#endif
#include <memory>

class IMessageBuilder {
public:
    virtual ~IMessageBuilder() = default;
    virtual std::unique_ptr<IMessage> build(const Json::Value& json) const = 0;
};
#pragma once
#include "IMessageBuilder.h"
#include <functional>
#include <memory> 

template <typename T>
class TemplateMessageBuilder : public IMessageBuilder {
public:
    explicit TemplateMessageBuilder(
        std::function<std::unique_ptr<T>(const Json::Value&)> factory
    ) : factory_(std::move(factory)) {
    }

    std::unique_ptr<IMessage> build(const Json::Value& json) const override {
        return factory_(json);
    }

private:
    std::function<std::unique_ptr<T>(const Json::Value&)> factory_;
};
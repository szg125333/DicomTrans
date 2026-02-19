// factory/MessageBuilderFactory.cpp
#include "../include/factory/MessageBuilderFactory.h"
#include "../include/core/IMessage.h"
#ifdef _WIN32
#include <json/json.h>
#else
#include <jsoncpp/json/json.h>
#endif
#include <stdexcept>
#include <unordered_map>

class InvalidMessageFormat : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class UnknownMessageType : public std::invalid_argument {
public:
    explicit UnknownMessageType(const std::string& type)
        : std::invalid_argument("Unknown message type: " + type) {}
};

// ======================
// 实现
// ======================

std::unique_ptr<IMessage> MessageBuilderFactory::create(const std::string& jsonStr) {
    if (jsonStr.empty()) {
        throw InvalidMessageFormat("Empty JSON string");
    }

    // 1. 解析 JSON
    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errs;
    std::istringstream s(jsonStr);
    if (!Json::parseFromStream(builder, s, &root, &errs)) {
        throw InvalidMessageFormat("JSON parse error: " + errs);
    }

    // 2. 检查 type 字段
    if (!root.isMember("type") || !root["type"].isString()) {
        throw InvalidMessageFormat("Missing or invalid 'type' field");
    }
    std::string typeName = root["type"].asString();

    // 3. 获取 builder 并构建消息
    try {
        IMessageBuilder* builder = getBuilder(typeName);
        return builder->build(root);
    }
    catch (const std::invalid_argument&) {
        // 将底层的 invalid_argument 转为更明确的 UnknownMessageType
        throw UnknownMessageType(typeName);
    }
}

void MessageBuilderFactory::registerBuilder(
    const std::string& typeName,
    std::unique_ptr<IMessageBuilder> builder
) {
    getRegistry()[typeName] = std::move(builder);
}

IMessageBuilder* MessageBuilderFactory::getBuilder(const std::string& typeName) {
    auto& reg = getRegistry();
    auto it = reg.find(typeName);
    if (it != reg.end()) {
        return it->second.get();
    }
    throw std::invalid_argument("Unknown message type: " + typeName);
}

std::unordered_map<std::string, std::unique_ptr<IMessageBuilder>>&
MessageBuilderFactory::getRegistry() {
    static std::unordered_map<std::string, std::unique_ptr<IMessageBuilder>> registry;
    return registry;
}
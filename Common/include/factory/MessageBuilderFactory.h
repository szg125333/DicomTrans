// factory/MessageBuilderFactory.h
#pragma once
#include "../core/IMessageBuilder.h"
#include <memory>
#include <unordered_map>
#include <string>

class MessageBuilderFactory {
public:
    static std::unique_ptr<IMessage> create(const std::string& jsonStr);

    // 底层注册
    static void registerBuilder(
        const std::string& typeName,
        std::unique_ptr<IMessageBuilder> builder
    );

private:
    static IMessageBuilder* getBuilder(const std::string& typeName);
    static std::unordered_map<std::string, std::unique_ptr<IMessageBuilder>>& getRegistry();
};
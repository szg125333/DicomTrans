// src/messages/request/ImmutableFirstTypeRequest.cpp
#include "../include/messages/request/ImmutableFirstTypeRequest.h"
#ifdef _WIN32
#include <json/json.h>
#else
#include <jsoncpp/json/json.h>
#endif
#include "../include/core/TemplateMessageBuilder.h"
#include "../include/factory/MessageBuilderFactory.h"
#include <memory>

// 构造函数定义
ImmutableFirstTypeRequest::ImmutableFirstTypeRequest(
    double gantryAngle, const std::string& speed)
    : gantryAngle_(gantryAngle), speed_(speed) {
}

// toJson 实现
std::string ImmutableFirstTypeRequest::toJson() const {
    Json::Value root;
    root["type"] = TYPE_NAME;
    root["gantryAngle"] = gantryAngle_;
    root["speed"] = speed_;

    Json::StreamWriterBuilder builder;
    return Json::writeString(builder, root);
}

// ======================
// 注册到工厂
// ======================
namespace {
    auto createFromJson = [](const Json::Value& json) -> std::unique_ptr<ImmutableFirstTypeRequest> {
        if (!json.isMember("gantryAngle")) {
            throw std::invalid_argument("Missing 'gantryAngle' in req_MoveGantry");
        }
        double angle = json["gantryAngle"].asDouble();
        std::string speed = json.get("speed", "Normal").asString();
        return std::make_unique<ImmutableFirstTypeRequest>(angle, speed);
        };

    void registerMoveGantryRequest() {
        auto builder = std::make_unique<TemplateMessageBuilder<ImmutableFirstTypeRequest>>(createFromJson);
        MessageBuilderFactory::registerBuilder(ImmutableFirstTypeRequest::TYPE_NAME, std::move(builder));
    }

    struct Registrar {
        Registrar() { registerMoveGantryRequest(); }
    };
    static Registrar g_registrar;
}
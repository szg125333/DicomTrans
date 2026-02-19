#include "../include/messages/response/ImmutableFirstTypeResponse.h"
#include "../include/core/TemplateMessageBuilder.h"
#include "../include/factory/MessageBuilderFactory.h"
#ifdef _WIN32
#include <json/json.h>
#else
#include <jsoncpp/json/json.h>
#endif
#include <memory>
#include <stdexcept>


std::string ImmutableFirstTypeResponse::toJson() const {
    Json::Value root;
    root["type"] = getTypeName();
    root["requestId"] = requestId_;
    root["success"] = success_;
    root["actualAngle"] = actualAngle_;
    if (!errorMessage_.empty()) {
        root["errorMessage"] = errorMessage_;
    }
    Json::StreamWriterBuilder builder;
    return Json::writeString(builder, root);
}

// ======================
// 自动注册逻辑
// ======================

namespace {
    auto createFromJson = [](const Json::Value& json) -> std::unique_ptr<ImmutableFirstTypeResponse> {
        if (!json.isMember("requestId") || !json.isMember("success")) {
            throw std::invalid_argument("Missing required fields in rsp_MoveGantry");
        }

        std::string requestId = json["requestId"].asString();
        bool success = json["success"].asBool();
        double actualAngle = success && json.isMember("actualAngle")
            ? json["actualAngle"].asDouble() : 0.0;
        std::string errorMsg = (!success && json.isMember("errorMessage"))
            ? json["errorMessage"].asString() : "";

        return std::make_unique<ImmutableFirstTypeResponse>(
            std::move(requestId), success, actualAngle, std::move(errorMsg)
        );
        };

    void registerMoveGantryResponse() {
        auto builder = std::make_unique<TemplateMessageBuilder<ImmutableFirstTypeResponse>>(createFromJson);
        MessageBuilderFactory::registerBuilder(ImmutableFirstTypeResponse::TYPE_NAME, std::move(builder));
    }

    struct Registrar {
        Registrar() { registerMoveGantryResponse(); }
    };
    static Registrar g_registrar;
}
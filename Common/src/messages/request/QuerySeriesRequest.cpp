// src/messages/request/QuerySeriesRequest.cpp
#include "../include/messages/request/QuerySeriesRequest.h"
#include <json/json.h>
#include "../include/core/TemplateMessageBuilder.h"
#include "../include/factory/MessageBuilderFactory.h"
#include <memory>
#include <stdexcept>

// 构造函数
QuerySeriesRequest::QuerySeriesRequest(
    const std::string& patientId,
    const std::string& studyUid,
    const std::string& seriesUid)
    : patientId_(patientId), studyUid_(studyUid), seriesUid_(seriesUid) {
    if (patientId_.empty() || studyUid_.empty()) {
        throw std::invalid_argument("patientId and studyUid must not be empty");
    }
}

// 序列化为 JSON
std::string QuerySeriesRequest::toJson() const {
    Json::Value root;
    root["type"] = TYPE_NAME;
    root["patient_id"] = patientId_;
    root["study_uid"] = studyUid_;
    if (!seriesUid_.empty()) {
        root["series_uid"] = seriesUid_;
    }
    Json::StreamWriterBuilder builder;
    return Json::writeString(builder, root);
}

// ======================
// 从 JSON 反序列化 + 注册到工厂
// ======================
namespace {
    auto createFromJson = [](const Json::Value& json) -> std::unique_ptr<QuerySeriesRequest> {
        if (!json.isMember("patient_id") || !json.isMember("study_uid")) {
            throw std::invalid_argument("Missing 'patient_id' or 'study_uid' in req_QuerySeries");
        }

        std::string patientId = json["patient_id"].asString();
        std::string studyUid = json["study_uid"].asString();
        std::string seriesUid = json.get("series_uid", "").asString();

        if (patientId.empty() || studyUid.empty()) {
            throw std::invalid_argument("'patient_id' and 'study_uid' must not be empty");
        }

        return std::make_unique<QuerySeriesRequest>(patientId, studyUid, seriesUid);
        };

    void registerQuerySeriesRequest() {
        auto builder = std::make_unique<TemplateMessageBuilder<QuerySeriesRequest>>(createFromJson);
        MessageBuilderFactory::registerBuilder(QuerySeriesRequest::TYPE_NAME, std::move(builder));
    }

    struct Registrar {
        Registrar() { registerQuerySeriesRequest(); }
    };
    static Registrar g_registrar;
}
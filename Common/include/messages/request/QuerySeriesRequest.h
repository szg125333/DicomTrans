// src/messages/request/QuerySeriesRequest.h
#pragma once
#include "../../core/IMessage.h"
#include <string>

class QuerySeriesRequest : public IMessage {
public:
    // 协议标识
    static constexpr const char* TYPE_NAME = "req_QuerySeries";

    // 构造函数：必须提供 patient_id 和 study_uid，series_uid 可选
    QuerySeriesRequest(
        const std::string& patientId,
        const std::string& studyUid,
        const std::string& seriesUid = ""
    );

    std::string getTypeName() const override { return TYPE_NAME; }
    std::string toJson() const override;

    // Getter
    const std::string& getPatientId() const { return patientId_; }
    const std::string& getStudyUid() const { return studyUid_; }
    const std::string& getSeriesUid() const { return seriesUid_; }

private:
    std::string patientId_;
    std::string studyUid_;
    std::string seriesUid_; // 可为空，表示查询整个 Study 下所有 Series
};
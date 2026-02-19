#pragma once
#include "../../core/IMessage.h"
#include <string>

class ImmutableFirstTypeResponse : public IMessage {
public:
    ImmutableFirstTypeResponse(
        std::string requestId,
        bool success,
        double actualAngle,
        std::string errorMessage
    ) : requestId_(std::move(requestId)),
        success_(success),
        actualAngle_(actualAngle),
        errorMessage_(std::move(errorMessage))
    {
    }

    static constexpr const char* TYPE_NAME = "rsp_MoveGantry";
    static const char* staticTypeName() { return TYPE_NAME; }
    std::string getTypeName() const override { return TYPE_NAME; }

    std::string toJson() const override; 

    // Getter
    std::string getRequestId() const { return requestId_; }
    bool isSuccess() const { return success_; }
    double getActualAngle() const { return actualAngle_; }
    std::string getErrorMessage() const { return errorMessage_; }

private:
    std::string requestId_;
    bool success_ = false;
    double actualAngle_ = 0.0;
    std::string errorMessage_;
};
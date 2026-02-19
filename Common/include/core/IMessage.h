#pragma once
#include <string>
#include <memory>

class IMessage {
public:
    virtual ~IMessage() = default;
    virtual std::string getTypeName() const = 0;
    virtual std::string toJson() const =0;
};
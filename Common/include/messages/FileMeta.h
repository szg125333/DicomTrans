#pragma once
#include "../core/IMessage.h"
#include <string>
#ifdef _WIN32
#include <json/json.h>
#else
#include <jsoncpp/json/json.h>
#endif
class FileMeta : public IMessage {
public:
    static constexpr const char* TYPE_NAME = "FileMeta";

    FileMeta(const std::string& filename,
        int index,
        int total = 0);

    std::string getTypeName() const override { return TYPE_NAME; }

    // 序列化为 JSON
    std::string toJson() const override;

    // Getter
    const std::string& getFilename() const { return filename_; }
    int getIndex() const { return index_; }
    int getTotal() const { return total_; }

private:
    std::string filename_;
    int index_;
    int total_; // 总数，可选
};

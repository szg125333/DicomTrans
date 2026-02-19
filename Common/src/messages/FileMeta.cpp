#include "../include/messages/FileMeta.h"
#include "../include/core/TemplateMessageBuilder.h"
#include "../include/factory/MessageBuilderFactory.h"
#include <memory>
#include <stdexcept>

FileMeta::FileMeta(const std::string& filename, int index, int total)
    : filename_(filename), index_(index), total_(total)
{
}

std::string FileMeta::toJson() const
{
    Json::Value root;
    root["type"] = TYPE_NAME;
    root["filename"] = filename_;
    root["index"] = index_;
    if (total_ > 0) {
        root["total"] = total_;
    }
    Json::StreamWriterBuilder builder;
    return Json::writeString(builder, root);
}

// ======================
// 从 JSON 反序列化 + 注册到工厂
// ======================
namespace {
    auto createFromJson = [](const Json::Value& json) -> std::unique_ptr<FileMeta> {
        if (!json.isMember("filename") || !json.isMember("index")) {
            throw std::invalid_argument("Missing 'filename' or 'index' in resp_FileMeta");
        }

        std::string filename = json["filename"].asString();
        int index = json["index"].asInt();
        int total = json.get("total", 0).asInt();

        return std::make_unique<FileMeta>(filename, index, total);
        };

    void registerFileMeta() {
        auto builder = std::make_unique<TemplateMessageBuilder<FileMeta>>(createFromJson);
        MessageBuilderFactory::registerBuilder(FileMeta::TYPE_NAME, std::move(builder));
    }

    struct Registrar {
        Registrar() { registerFileMeta(); }
    };
    static Registrar g_registrar;
}



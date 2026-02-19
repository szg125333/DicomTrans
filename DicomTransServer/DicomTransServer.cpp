// main.cpp (PacsServer 入口)
#include <zmq.h>
#include "factory/MessageBuilderFactory.h"
#include "messages/request/QuerySeriesRequest.h"
#include "messages/FileMeta.h"
#include "Worker/NewNotifyWorker.h"
#include "Worker/NewRequestWorker.h"
#include <json/json.h>
#include <iostream>
#include <filesystem>
#include <csignal>
#include <atomic>
#include <fstream>

// 全局运行标志
std::atomic<bool> running(true);

void signalHandler(int) {
    running = false;
}

bool isValidStoragePath(const std::string& path, const std::string& base) {
    try {
        auto canonicalBase = std::filesystem::canonical(base);
        auto canonicalPath = std::filesystem::canonical(path);
        return std::filesystem::equivalent(canonicalBase, canonicalPath) ||
            (canonicalPath.string().rfind(canonicalBase.string(), 0) == 0);
    }
    catch (...) {
        return false; // 如果路径不存在或解析失败，直接判定为非法
    }
}


void sendErrorResponse(void* socket, const std::string& msg) {
    Json::Value error;
    error["error"] = msg;
    Json::StreamWriterBuilder builder;
    std::string jsonStr = Json::writeString(builder, error);
    zmq_send(socket, jsonStr.c_str(), jsonStr.size(), 0);
}

void handleQuerySeries(void* socket, const QuerySeriesRequest* req) {
    std::string basePath = "/root/PacsServer/Storage"; // 改成绝对路径
    std::string seriesPath = basePath + "/" +
        req->getPatientId() + "/" +
        req->getStudyUid() + "/" +
        (req->getSeriesUid().empty() ? "" : req->getSeriesUid());

    std::cout << "[INFO] Handling QuerySeriesRequest for series path: " << seriesPath << "\n";

    // 使用封装好的函数
    if (!isValidStoragePath(seriesPath, basePath)) {
        sendErrorResponse(socket, "Invalid path");
        return;
    }

    if (!std::filesystem::exists(seriesPath)) {
        sendErrorResponse(socket, "Series not found");
        return;
    }

    // Step 1: 收集所有 .dcm 文件路径
    std::vector<std::filesystem::path> dcmFiles;
    for (const auto& entry : std::filesystem::directory_iterator(seriesPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".dcm") {
            dcmFiles.push_back(entry.path());
        }
    }

    if (dcmFiles.empty()) {
        sendErrorResponse(socket, "No .dcm files");
        return;
    }

    // Step 2: 逐个发送，判断是否最后一个
    for (size_t i = 0; i < dcmFiles.size(); ++i) {
        const auto& path = dcmFiles[i];
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            sendErrorResponse(socket, "Failed to open file");
            continue;
        }
        // 再发二进制
        std::vector<char> buffer((std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>());
        int flags = (i == dcmFiles.size() - 1) ? 0 : ZMQ_SNDMORE;
        zmq_send(socket, buffer.data(), buffer.size(), flags);
    }
}

int main() {
    // 注册 Ctrl+C 信号处理
    std::signal(SIGINT, signalHandler);

    void* ctx = zmq_ctx_new();
    void* socket = zmq_socket(ctx, ZMQ_REP);
    zmq_bind(socket, "tcp://*:5557");
    std::cout << "[INFO] PACS Server listening on port 5557...\n";

    zmq_pollitem_t items[] = { { socket, 0, ZMQ_POLLIN, 0 } };

    while (running) {
        int rc = zmq_poll(items, 1, 1000); // 1秒超时
        if (rc > 0 && (items[0].revents & ZMQ_POLLIN)) {
            zmq_msg_t msg;
            zmq_msg_init(&msg);
            if (zmq_msg_recv(&msg, socket, 0) == -1) {
                zmq_msg_close(&msg);
                std::cerr << "[WARN] Failed to receive message\n";
                continue;
            }

            std::string jsonStr(static_cast<char*>(zmq_msg_data(&msg)), zmq_msg_size(&msg));
            zmq_msg_close(&msg);

            std::cout << "[INFO] Received request: " << jsonStr << "\n";

            try {
                auto request = MessageBuilderFactory::create(jsonStr);
                std::cout << "[DEBUG] Request type: " << request->getTypeName() << "\n";

                if (request->getTypeName() == QuerySeriesRequest::TYPE_NAME) {
                    std::cout << "[INFO] Handling QuerySeriesRequest...\n";
                    handleQuerySeries(socket, dynamic_cast<QuerySeriesRequest*>(request.get()));
                    std::cout << "[INFO] Finished handling QuerySeriesRequest\n";
                }
                else {
                    std::cerr << "[ERROR] Unsupported message type: "
                        << request->getTypeName() << "\n";
                    sendErrorResponse(socket, "Unsupported message type");
                }
            }
            catch (const std::exception& e) {
                std::cerr << "[ERROR] Exception: " << e.what() << "\n";
                sendErrorResponse(socket, e.what());
            }
        }
    }

    std::cout << "[INFO] Shutting down PACS Server...\n";
    zmq_close(socket);
    zmq_ctx_destroy(ctx);
    return 0;
}



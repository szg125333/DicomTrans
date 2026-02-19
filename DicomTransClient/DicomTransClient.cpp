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

std::string patientId = "HFP";
std::string studyUid = "1.2.246.352.221.5319850929801793938.489836017520611990";
std::string seriesUid = "1.2.246.352.221.5381620381804819935.12304672186111552956";
std::filesystem::path basePath = "Storage";
std::filesystem::path seriesPath = basePath / patientId / studyUid / seriesUid;

int main() {
    // 创建 ZMQ 上下文和 REQ 套接字
    void* ctx = zmq_ctx_new();
    void* socket = zmq_socket(ctx, ZMQ_REQ);

    // 连接到服务器
    zmq_connect(socket, "tcp://47.100.193.91:5557");

    auto str = std::make_unique<QuerySeriesRequest>("HFP",
        "1.2.246.352.221.5319850929801793938.489836017520611990", "1.2.246.352.221.5381620381804819935.12304672186111552956");
    std::filesystem::create_directories(seriesPath);

    //Json::StreamWriterBuilder builder;
    //std::string jsonStr = Json::writeString(builder, str->toJson());

    // 发送请求
    zmq_send(socket, str->toJson().c_str(), str->toJson().size(), 0);

    // 接收服务器返回（可能是多帧：meta + 二进制）
    int fileIndex = 0;
    while (true) {
        zmq_msg_t msg;
        zmq_msg_init(&msg);
        int size = zmq_msg_recv(&msg, socket, 0);
        if (size == -1) {
            zmq_msg_close(&msg);
            break;
        }

        size_t dataSize = zmq_msg_size(&msg);
        void* data = zmq_msg_data(&msg);

        // 保存为文件
        std::string filename = (seriesPath / ("received_" + std::to_string(fileIndex++) + ".dcm")).string();
        std::ofstream outFile(filename, std::ios::binary);
        if (!outFile.is_open()) {
            std::cerr << "[ERROR] Cannot create file: " << filename << "\n";
        }
        else {
            outFile.write(static_cast<const char*>(data), dataSize);
            outFile.close();
            std::cout << "[CLIENT] Saved " << dataSize << " bytes to " << filename << "\n";
        }
        // 检查是否还有更多帧
        int more = 0;
        size_t moreSize = sizeof(more);
        zmq_getsockopt(socket, ZMQ_RCVMORE, &more, &moreSize);

        zmq_msg_close(&msg);

        if (!more) {
            std::cout << "[CLIENT] All frames received. Total files: " << fileIndex << "\n";
            break;
        }
    }

    zmq_close(socket);
    zmq_ctx_destroy(ctx);
    return 0;
}



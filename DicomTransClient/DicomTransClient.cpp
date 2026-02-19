// main.cpp (PacsServer 入口)
#include <zmq.h>
#include "factory/MessageBuilderFactory.h"
#include "messages/request/QuerySeriesRequest.h"
#include "messages/FileMeta.h"
#include "Worker/NewNotifyWorker.h"
#include "Worker/NewRequestWorker.h"
#include <iostream>
#include <filesystem>
#include <csignal>
#include <atomic>
#include <fstream>
#include <stdexcept>

namespace fs = std::filesystem;

#ifdef _WIN32
#include <windows.h>
#include <cstdlib> // _pgmptr
#include <json/json.h>
#else
#include <unistd.h> // readlink
#include <jsoncpp/json/json.h>

#endif
#include <zmq.h>

#define MAX_LENGTH          260

std::string patientId = "HFP";
std::string studyUid = "1.2.246.352.221.5319850929801793938.489836017520611990";
std::string seriesUid = "1.2.246.352.221.5381620381804819935.12304672186111552956";
std::filesystem::path basePath = "Storage";
std::filesystem::path seriesPath = basePath / patientId / studyUid / seriesUid;

/**
 * @brief 跨平台获取可执行程序所在的目录
 * @return exe所在目录的绝对路径
 * @throw std::runtime_error 获取失败时抛出异常
 */
fs::path getExeDirectory() {
    fs::path exePath;

#ifdef _WIN32
    // Windows平台：通过GetModuleFileName获取exe完整路径
    // 比_pgmptr更可靠，避免某些编译环境下_pgmptr失效
    char buffer[MAX_LENGTH] = { 0 };
    GetModuleFileNameA(NULL, buffer, MAX_LENGTH);
    exePath = fs::canonical(buffer);
#else
    // Linux平台：通过/proc/self/exe获取exe路径
    char buffer[MAX_LENGTH] = { 0 };
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len == -1) {
        throw std::runtime_error("获取Linux可执行程序路径失败");
    }
    exePath = fs::canonical(std::string(buffer, len));
#endif

    // 返回exe所在的目录（去掉exe文件名）
    return exePath.parent_path();
}

int main() {

    fs::path exeDir = getExeDirectory();
    std::cout << "可执行程序所在目录：" << exeDir << std::endl;

    // 创建 ZMQ 上下文和 REQ 套接字
    void* ctx = zmq_ctx_new();
    void* socket = zmq_socket(ctx, ZMQ_REQ);

    // 连接到服务器
    zmq_connect(socket, "tcp://47.100.193.91:5557");

    auto str = std::make_unique<QuerySeriesRequest>("HFP",
        "1.2.246.352.221.5319850929801793938.489836017520611990", "1.2.246.352.221.5381620381804819935.12304672186111552956");
    fs::path seriesPath = exeDir / "Storage" / patientId / studyUid / seriesUid;

    // 创建多级目录
    if (!fs::exists(seriesPath)) {
        fs::create_directories(seriesPath);
        std::cout << "存储目录已创建：" << seriesPath << std::endl;
    }
    else {
        std::cout << "存储目录已存在：" << seriesPath << std::endl;
    }

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



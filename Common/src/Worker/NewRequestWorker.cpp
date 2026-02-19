// NewRequestWorker.cpp
#include "../include/Worker/NewRequestWorker.h"
#include "zmq.h"
#include "../include/Worker/RequestIdCreator.h"           // 用于生成唯一 requestId
#include "../include/factory/MessageBuilderFactory.h"
#include "ZmqAddresses.h"
#include <iostream>
#include <chrono>
#include <cstring>
#include <json/json.h>

// 静态成员定义
std::unique_ptr<IMessage> NewRequestWorker::pendingRequest_ = nullptr;
std::atomic<bool> NewRequestWorker::hasPendingRequest_{ false };

NewRequestWorker::NewRequestWorker(void* zmqContext)
    : context_(zmqContext) {
}

NewRequestWorker::~NewRequestWorker() {
    stop();
}

void NewRequestWorker::start() {
    if (!running_.exchange(true)) {
        workerThread_ = std::thread(&NewRequestWorker::run, this);
    }
}

void NewRequestWorker::stop() {
    if (running_.exchange(false)) {
        if (workerThread_.joinable()) {
            workerThread_.join();
        }
    }
}

void NewRequestWorker::sendRequest(std::unique_ptr<IMessage> request) {
    // 原子操作：如果当前没有待发送消息，则设置新消息
    if (!hasPendingRequest_.exchange(true)) {
        pendingRequest_ = std::move(request);
    }
    // 如果已有待发送消息，新消息会被丢弃（可根据需求改为队列）
}

void NewRequestWorker::run() {
    // 创建 REQ socket（请求-响应模式）
    void* socket = zmq_socket(context_, ZMQ_SUB);
    zmq_connect(socket, ZmqAddresses::REQUEST_REPLY_URL.c_str());

    std::cout << "[NewRequestWorker] Connected to " << ZmqAddresses::REQUEST_REPLY_URL << "\n";

    // 主循环：定期检查是否有新请求要发送
    while (running_.load()) {
        if (hasPendingRequest_.load()) {
            sendInternal(socket);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    zmq_close(socket);
}

void NewRequestWorker::sendInternal(void* socket) {
    // 获取待发送消息（移动语义避免拷贝）
    auto localMsg = std::move(pendingRequest_);
    hasPendingRequest_.store(false); // 标记为空
    if (!localMsg) return;

    // 步骤1: 将 IMessage 转为 JSON，并注入 requestId
    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(localMsg->toJson(), root)) {
        std::cerr << "[NewRequestWorker] Failed to parse request JSON\n";
        return;
    }
    root["requestId"] = RequestIdCreator::generateRequestId(); // 注入唯一ID

    // 序列化为字符串（紧凑格式）
    Json::StreamWriterBuilder wbuilder;
    wbuilder["indentation"] = ""; // 禁用缩进，减小体积
    std::string jsonStr = Json::writeString(wbuilder, root);

    std::cout << "[SEND REQUEST] " << jsonStr << "\n";

    // 步骤2: 发送纯 JSON 字符串（无 envelope）
    zmq_send(socket, jsonStr.c_str(), jsonStr.size(), 0);

    // 步骤3: 接收响应（必须 recv，否则 REQ socket 会阻塞）
    char buffer[4096];
    int size = zmq_recv(socket, buffer, sizeof(buffer) - 1, 0);
    if (size <= 0) {
        std::cerr << "[RECV] No response from server\n";
        return;
    }
    buffer[size] = '\0'; // 确保字符串以 null 结尾

    std::string replyJson(buffer, size);

    // 步骤4: 反序列化响应并打印
    try {
        auto response = MessageBuilderFactory::create(replyJson);
        std::cout << "[RECV REPLY] Type: " << response->getTypeName()
            << " | JSON: " << replyJson << "\n";
    }
    catch (const std::exception& e) {
        std::cerr << "[PARSE ERROR] Failed to parse reply: " << e.what() << "\n";
        std::cerr << "[RAW REPLY] " << replyJson << "\n";
    }
}
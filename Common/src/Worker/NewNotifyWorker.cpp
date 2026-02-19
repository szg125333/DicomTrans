// NewNotifyWorker.cpp
#include "../include/Worker/NewNotifyWorker.h"
#include "zmq.h"
#include "../include/factory/MessageBuilderFactory.h"
#include "ZmqAddresses.h"
#include <iostream>
#include <cstring>

NewNotifyWorker::NewNotifyWorker(void* zmqContext)
    : context_(zmqContext) {
}

NewNotifyWorker::~NewNotifyWorker() {
    stop();
}

void NewNotifyWorker::start() {
    if (!running_.exchange(true)) {
        workerThread_ = std::thread(&NewNotifyWorker::run, this);
    }
}

void NewNotifyWorker::stop() {
    if (running_.exchange(false)) {
        if (workerThread_.joinable()) {
            workerThread_.join();
        }
    }
}

void NewNotifyWorker::run() {
    // 创建 SUB socket（订阅模式）
    void* socket = zmq_socket(context_, ZMQ_REP);
    zmq_connect(socket, ZmqAddresses::NOTIFICATION_URL.c_str());

    // 订阅所有消息（空过滤器）
    zmq_setsockopt(socket, ZMQ_SUBSCRIBE, "", 0);

    std::cout << "[NewNotifyWorker] Listening for notifications on "
        << ZmqAddresses::NOTIFICATION_URL << "\n";

    // 使用 poll 实现可中断的接收（避免 zmq_recv 永久阻塞）
    zmq_pollitem_t items[1];
    items[0].socket = socket;
    items[0].events = ZMQ_POLLIN;

    while (running_.load()) {
        // 每 1 秒检查一次 running_ 状态
        int n = zmq_poll(items, 1, 1000); // 1000ms 超时
        if (n == -1) {
            std::cerr << "[NewNotifyWorker] zmq_poll failed\n";
            break;
        }

        if (n == 1 && (items[0].revents & ZMQ_POLLIN)) {
            handleNotification(socket);
        }
    }

    zmq_close(socket);
}

void NewNotifyWorker::handleNotification(void* socket) {
    // 接收纯 JSON 字符串（无 envelope）
    char buffer[4096];
    int size = zmq_recv(socket, buffer, sizeof(buffer) - 1, 0);
    if (size <= 0) {
        std::cerr << "[NewNotifyWorker] zmq_recv failed or empty\n";
        return;
    }
    buffer[size] = '\0';

    std::string json(buffer, size);

    // 尝试反序列化为 IMessage 并打印
    try {
        auto notify = MessageBuilderFactory::create(json);
        std::cout << "[NOTIFICATION] Type: " << notify->getTypeName()
            << " | JSON: " << json << "\n";
    }
    catch (const std::exception& e) {
        std::cerr << "[NOTIFICATION PARSE ERROR] " << e.what() << "\n";
        std::cerr << "[RAW NOTIFICATION] " << json << "\n";
    }
}
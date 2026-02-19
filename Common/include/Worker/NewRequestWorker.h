// NewRequestWorker.h
#pragma once
#include <memory>
#include <thread>
#include <atomic>
#include "../core/IMessage.h"  // 假设你已有 IMessage 基类

/// @brief 负责发送新架构请求（基于 IMessage）并接收响应
///        使用 ZeroMQ REQ 模式，每次 send 后必须 recv
class NewRequestWorker {
public:
    /// @brief 构造函数
    /// @param zmqContext 已创建的 ZeroMQ 上下文（由外部管理生命周期）
    explicit NewRequestWorker(void* zmqContext);

    /// @brief 析构函数，自动停止线程
    ~NewRequestWorker();

    /// @brief 启动后台工作线程
    void start();

    /// @brief 停止并等待线程结束
    void stop();

    /// @brief 异步发送一个请求消息（线程安全）
    ///         内部会注入 requestId 并转为 JSON 发送
    /// @param request 非空的 IMessage 派生类实例（如 ImmutableFirstTypeRequest）
    void sendRequest(std::unique_ptr<IMessage> request);

private:
    /// @brief 后台线程主循环
    void run();

    /// @brief 实际执行一次请求-响应交互
    /// @param socket 已连接的 REQ socket
    void sendInternal(void* socket);

    // 成员变量
    void* context_;                    ///< ZeroMQ 上下文指针
    std::thread workerThread_;         ///< 后台工作线程
    std::atomic<bool> running_{ false }; ///< 控制线程运行状态

    // 静态成员：用于跨线程传递待发送的消息（单生产者-单消费者）
    static std::unique_ptr<IMessage> pendingRequest_;
    static std::atomic<bool> hasPendingRequest_;
};
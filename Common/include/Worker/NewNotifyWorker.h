// NewNotifyWorker.h
#pragma once
#include <memory>
#include <thread>
#include <atomic>
#include "../core/IMessage.h"

/// @brief 负责监听并打印新架构的通知消息（基于 IMessage）
///        使用 ZeroMQ SUB 模式，只接收不发送
class NewNotifyWorker {
public:
    /// @brief 构造函数
    /// @param zmqContext 已创建的 ZeroMQ 上下文
    explicit NewNotifyWorker(void* zmqContext);

    /// @brief 析构函数，自动停止线程
    ~NewNotifyWorker();

    /// @brief 启动后台监听线程
    void start();

    /// @brief 停止并等待线程结束
    void stop();

private:
    /// @brief 后台线程主循环
    void run();

    /// @brief 处理一条收到的通知消息
    /// @param socket 已连接的 SUB socket
    void handleNotification(void* socket);

    // 成员变量
    void* context_;                    ///< ZeroMQ 上下文指针
    std::thread workerThread_;         ///< 后台监听线程
    std::atomic<bool> running_{ false }; ///< 控制线程运行状态
};
// ZmqAddresses.h
#pragma once
#include <string>

/// @brief ZeroMQ 连接地址配置（硬编码，便于独立部署）
namespace ZmqAddresses {
    /// 请求-响应模式的地址（REQ/REP）
    const std::string REQUEST_REPLY_URL = "tcp://47.100.193.91:5557";

    /// 通知订阅地址（PUB/SUB）
    const std::string NOTIFICATION_URL = "tcp://47.100.193.91:5557";
}
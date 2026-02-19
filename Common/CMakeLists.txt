add_library(Common STATIC
    src/messages/request/ImmutableFirstTypeRequest.cpp
    src/messages/request/QuerySeriesRequest.cpp
    src/messages/response/ImmutableFirstTypeResponse.cpp
    src/factory/MessageBuilderFactory.cpp
    src/Worker/NewNotifyWorker.cpp
    src/Worker/NewRequestWorker.cpp
    src/messages/FileMeta.cpp
)

target_include_directories(Common PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# 可选：安装头文件
install(DIRECTORY include/ DESTINATION include)
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

if(WIN32)
    # Windows 下手动指定 .lib
    target_link_directories(Common PRIVATE
        $<$<CONFIG:Debug>:C:/Workspace/GitRepository/libzmq4.3.5_x64/lib/Debug>
        $<$<CONFIG:Release>:C:/Workspace/GitRepository/libzmq4.3.5_x64/lib/Release>
        $<$<CONFIG:Debug>:C:/Workspace/GitRepository/jsoncpp1.9.5_x64/lib/Debug>
        $<$<CONFIG:Release>:C:/Workspace/GitRepository/jsoncpp1.9.5_x64/lib/Release>
    )
    target_link_libraries(Common
        libzmq.lib
        jsoncpp.lib
    )
else()
    # Linux 下用 pkg-config 自动查找 .so
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(ZMQ REQUIRED libzmq)
    pkg_check_modules(JSONCPP REQUIRED jsoncpp)

    target_include_directories(Common PRIVATE
        ${ZMQ_INCLUDE_DIRS}
        ${JSONCPP_INCLUDE_DIRS}
    )
    target_link_libraries(Common PRIVATE
        ${ZMQ_LIBRARIES}
        ${JSONCPP_LIBRARIES}
    )

endif()


# 可选：安装头文件
install(DIRECTORY include/ DESTINATION include)
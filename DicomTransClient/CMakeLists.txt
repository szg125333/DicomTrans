add_executable(DicomTransClient DicomTransClient.cpp)

if(WIN32)
    # Windows 下手动指定 .lib
    target_link_directories(DicomTransClient PRIVATE
        $<$<CONFIG:Debug>:C:/Workspace/GitRepository/libzmq4.3.5_x64/lib/Debug>
        $<$<CONFIG:Release>:C:/Workspace/GitRepository/libzmq4.3.5_x64/lib/Release>
        $<$<CONFIG:Debug>:C:/Workspace/GitRepository/jsoncpp1.9.5_x64/lib/Debug>
        $<$<CONFIG:Release>:C:/Workspace/GitRepository/jsoncpp1.9.5_x64/lib/Release>
    )
    target_link_libraries(DicomTransClient
        libzmq.lib
        jsoncpp.lib
		Common.lib
    )
else()
    # Linux 下用 pkg-config 自动查找 .so
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(ZMQ REQUIRED libzmq)
    pkg_check_modules(JSONCPP REQUIRED jsoncpp)

    target_include_directories(DicomTransClient PRIVATE
        ${ZMQ_INCLUDE_DIRS}
        ${JSONCPP_INCLUDE_DIRS}
    )
    target_link_libraries(DicomTransClient PRIVATE
        ${ZMQ_LIBRARIES}
        ${JSONCPP_LIBRARIES}
		Common
    )

endif()
# 设置包含目录
target_include_directories(DicomTransClient PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/Common/include
)
add_executable(DicomTransServer DicomTransServer.cpp)

if(WIN32)
    # Windows 下手动指定 .lib
    target_link_directories(DicomTransServer PRIVATE
        $<$<CONFIG:Debug>:C:/Workspace/GitRepository/libzmq4.3.5_x64/lib/Debug>
        $<$<CONFIG:Release>:C:/Workspace/GitRepository/libzmq4.3.5_x64/lib/Release>
        $<$<CONFIG:Debug>:C:/Workspace/GitRepository/jsoncpp1.9.5_x64/lib/Debug>
        $<$<CONFIG:Release>:C:/Workspace/GitRepository/jsoncpp1.9.5_x64/lib/Release>
    )
    target_link_libraries(DicomTransServer
        libzmq.lib
        jsoncpp.lib
    )
else()
    # Linux 下用 pkg-config 自动查找 .so
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(ZMQ REQUIRED libzmq)
    pkg_check_modules(JSONCPP REQUIRED jsoncpp)

    target_include_directories(DicomTransServer PRIVATE
        ${ZMQ_INCLUDE_DIRS}
        ${JSONCPP_INCLUDE_DIRS}
    )
    target_link_libraries(DicomTransServer PRIVATE
        ${ZMQ_LIBRARIES}
        ${JSONCPP_LIBRARIES}
		Common
    )

endif()

target_include_directories(DicomTransServer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/Common/include
)
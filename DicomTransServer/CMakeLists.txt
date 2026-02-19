add_executable(DicomTransServer DicomTransServer.cpp)

target_link_libraries(DicomTransServer
    Common
    zmq::zmq
)

target_include_directories(DicomTransServer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/Common/include
)
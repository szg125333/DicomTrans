cmake_minimum_required(VERSION 3.18)
project(DicomTrans)

set(CMAKE_CXX_STANDARD 17)


# 添加子目录
add_subdirectory(Common)
add_subdirectory(DicomTransClient)
add_subdirectory(DicomTransServer)
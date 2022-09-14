cmake_minimum_required(VERSION 3.0 )
project(svm)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

include_directories(
    ${OpenCV_INCLUDE_DIRS})

add_executable(svm src/SvmClassifier.cpp)

target_link_libraries(svm  ${OpenCV_LIBS})



cmake_minimum_required(VERSION 3.0 )
project(estimator)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

include_directories(
    ${OpenCV_INCLUDE_DIRS})

add_executable(estimator src/main.cpp
                   src/utils/Log.cpp
                   src/utils/Timer.cpp
                   src/utils/Lane.cpp
                   src/utils/FileUtils.cpp
                   src/utils/CmdLineParser.cpp
                   src/detector/YoloDetector.cpp
                   src/detector/MobilenetDetector.cpp
                   src/tracker/VehicleTracker.cpp
                   src/estimator/TrafficEstimator.cpp)

target_link_libraries(estimator  ${OpenCV_LIBS})



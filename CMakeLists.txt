cmake_minimum_required(VERSION 3.0 )
project(app)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

include_directories(
    ${OpenCV_INCLUDE_DIRS})

add_executable(app src/main.cpp
                   src/utils/Timer.cpp
                   src/utils/Lane.cpp
                   src/utils/FileUtils.cpp
                   src/detector/YoloDetector.cpp
                   src/detector/MobilenetDetector.cpp
                   src/tracker/VehicleTracker.cpp
                   src/estimator/TrafficEstimator.cpp)

target_link_libraries(app  ${OpenCV_LIBS})



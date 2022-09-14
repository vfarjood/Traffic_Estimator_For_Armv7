#include <iostream>
#include <vector>
#include <map>

#include "utils/Timer.hpp"
#include "utils/Lane.hpp"
#include "utils/FileUtils.hpp"
#include "entity/Result.hpp"
#include "detector/YoloDetector.hpp"
#include "detector/MobilenetDetector.hpp"
#include "tracker/VehicleTracker.hpp"
#include "estimator/TrafficEstimator.hpp"

constexpr int MODEL = 1; // YOLOv5 = 1; MobileNet_SSDv2 = 2
std::string YOLO_CONFIG[2] = { "../models/yolo/yolov5n.onnx",
                                 "../models/yolo/classes.txt"};

std::string MOBILENET_CONFIG[3] = {"../models/mobilenet/frozen_inference_graph.pb",
                                     "../models/mobilenet/ssd_mobilenet_v2_coco_2018_03_29.pbtxt",
                                        "../models/mobilenet/classes.txt"};

constexpr int INPUT_WIDTH = 640;
constexpr int INPUT_HEIGHT = 640;

int main(int argc, char** argv )
{
    // std::cout << "OpenCV version : " << CV_VERSION << std::endl;
    cv::Size image_size = {INPUT_WIDTH, INPUT_HEIGHT};
    Timer time, total_time;
    float detection_time = 0.0f;
    float tracking_time = 0.0f;
    float estimation_time = 0.0f;
    total_time.start();
    

    std::string data_path = "../media/dataset/road_1/";
    // load input image list:
    std::vector<std::string> input_files= {data_path + "medium/img3.jpg", data_path + "medium/img4.jpg"};
    // keep all detected vehicles in a vector for both images:
    std::vector<std::vector<Centroid>> vector_of_vehicles; 
    vector_of_vehicles.reserve(50);

    //Detect all the vehicles:
    time.start();
    if(MODEL == 1){
        YoloDetector detector;
        detector.predict(vector_of_vehicles, YOLO_CONFIG[0], YOLO_CONFIG[1], 
                            input_files, INPUT_WIDTH, INPUT_HEIGHT);
    } 
    else if(MODEL == 2){
        MobilenetDetector detector;
        detector.predict(vector_of_vehicles, MOBILENET_CONFIG[0], MOBILENET_CONFIG[1], MOBILENET_CONFIG[2], 
                            input_files, INPUT_WIDTH, INPUT_HEIGHT);
    } 
    else {
        std::cout << "Incorrect MODEL code!\n " ;
        return -1;
    }
    detection_time = time.stop();

    // Find Vehicles Trajectory:
    Lane lines(data_path + "lines.txt");
    time.start();
    std::vector<Centroid> tracked_vehicles;
    VehicleTracker tracker;
    tracker.track(vector_of_vehicles, tracked_vehicles, lines, image_size);
    tracking_time = time.stop();
    

    // Traffic estimation:
    time.start();
    Result traffic;
    TrafficEstimator estimator;
    traffic = estimator.estimate(tracked_vehicles, lines, image_size);
    estimation_time = time.stop();

    std::cout << "Computation Time for " << input_files.size() << " frames:\n";
    std::cout << "    -Detection     = " << std::fixed << std::setprecision(4)<< detection_time << "s\n";
    std::cout << "    -Tracking      = " << std::fixed << std::setprecision(4)<< tracking_time << "s\n";
    std::cout << "    -Estimation    = " << std::fixed << std::setprecision(4)<< estimation_time << "s\n";
    std::cout << "    -Total         = " << std::fixed << std::setprecision(4)<< total_time.stop() << "s\n";

    // Write to result file:
    FileUtils::save(tracked_vehicles, traffic);
    FileUtils::drawResultOnImage(input_files, lines, vector_of_vehicles, INPUT_WIDTH, INPUT_HEIGHT);

    return 0;
}

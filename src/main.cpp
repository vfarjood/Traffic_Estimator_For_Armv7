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
constexpr int INPUT_WIDTH = 640;
constexpr int INPUT_HEIGHT = 640;

int main(int argc, char** argv )
{
    // std::cout << "OpenCV version : " << CV_VERSION << std::endl;
    cv::Size image_size = {INPUT_WIDTH, INPUT_HEIGHT};
    Timer time, total_time;
    float total_image_load_time = 0.0f;
    float model_loading_time = 0.0f;
    float detection_time = 0.0f;
    float tracking_time = 0.0f;
    float estimation_time = 0.0f;
    float saving_time = 0.0f;
    total_time.start();
    

    std::string data_path = "../media/dataset/road_1/";
    // load input image list:
    std::vector<std::string> input_files= {data_path + "medium/img3.jpg", data_path + "medium/img4.jpg"};
    // keep all detected vehicles in a vector for both images:
    std::vector<std::vector<Centroid>> vector_of_vehicles; 
    vector_of_vehicles.reserve(50);

    //Detect all the vehicles:
    if(MODEL == 1){
        YoloDetector detector;
        std::string classes_path = "../models/yolo/classes.txt";
        std::string model_path = "../models/yolo/yolov5n.onnx";
        detector.predict(vector_of_vehicles, model_path, classes_path, input_files, INPUT_WIDTH, INPUT_HEIGHT);
    } else if(MODEL == 2){
        MobilenetDetector detector;
        std::string classes_path = "../models/mobilenet/classes.txt";
        std::string model_path = "../models/mobilenet/frozen_inference_graph.pb";
        std::string model_config = "../models/mobilenet/ssd_mobilenet_v2_coco_2018_03_29.pbtxt";
        detector.predict(vector_of_vehicles, model_path, model_config, 
                            classes_path, input_files, INPUT_WIDTH, INPUT_HEIGHT);
    } else {
        std::cout << "Incorrect MODEL code!\n " ;
        return -1;
    }

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
    

    // Write to result file:
    time.start();
    FileUtils::save(tracked_vehicles, traffic);
    saving_time = time.stop();


    std::cout << "Computation Time for " << input_files.size() << " frames:\n";
    std::cout << "    -Loading Image = " << std::fixed << std::setprecision(4)<< total_image_load_time << "s\n";
    std::cout << "    -Loading Model = " << std::fixed << std::setprecision(4)<< model_loading_time << "s\n";
    std::cout << "    -Detection     = " << std::fixed << std::setprecision(4)<< detection_time << "s\n";
    std::cout << "    -Tracking      = " << std::fixed << std::setprecision(4)<< tracking_time << "s\n";
    std::cout << "    -Estimation    = " << std::fixed << std::setprecision(4)<< estimation_time << "s\n";
    std::cout << "    -Saving        = " << std::fixed << std::setprecision(4)<< saving_time << "s\n";
    std::cout << "    -Total         = " << std::fixed << std::setprecision(4)<< total_time.stop() << "s\n";


    FileUtils::drawResultOnImage(input_files, lines, vector_of_vehicles, INPUT_WIDTH, INPUT_HEIGHT);

    return 0;
}

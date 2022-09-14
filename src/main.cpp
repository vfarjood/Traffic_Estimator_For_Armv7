#include <iostream>
#include <vector>
#include <map>
#include "utils/Log.hpp"
#include "utils/Timer.hpp"
#include "utils/Lane.hpp"
#include "utils/FileUtils.hpp"
#include "entity/Result.hpp"
#include "entity/Parameters.hpp"
#include "detector/YoloDetector.hpp"
#include "detector/MobilenetDetector.hpp"
#include "tracker/VehicleTracker.hpp"
#include "estimator/TrafficEstimator.hpp"
#include "utils/CmdLineParser.hpp"

constexpr int INPUT_WIDTH = 640;
constexpr int INPUT_HEIGHT = 640;

int main(int argc, char** argv )
{
    LOG_INFO("Main:          Program has started.");
    Timer total_time;
    total_time.start();

    
    CmdLineParser parser(argc, argv);
    Parameters param;
    param = parser.getParameters();

    LOG_UPDATE_CONFIGURE(parser.get<std::string>("logMode"),
                            parser.get<std::string>("logType"),
                                parser.get<std::string>("logLevel")); 
    
    
    // load input image list:
    std::vector<std::string> input_files= {param.data_path + "img1.jpg", param.data_path + "img2.jpg"};
    // keep all detected vehicles in a vector for both images:
    std::vector<std::vector<Centroid>> vector_of_vehicles; 
    vector_of_vehicles.reserve(50);

    //Detect all the vehicles:
    Result traffic;
    Timer time;
    time.start();
    if(param.model_name == "yolo"){
        LOG_INFO("Main:          YOLOv5n model is selected as object detector.");
        YoloDetector detector;
        detector.predict(vector_of_vehicles, param.yolo_model, param.yolo_class, 
                            input_files, INPUT_WIDTH, INPUT_HEIGHT);
    } 
    else if(param.model_name == "mobilenet"){
        LOG_INFO("Main:          MobilNet_SSDv2 model is selected as object detector.");
        MobilenetDetector detector;
        detector.predict(vector_of_vehicles, param.mobilenet_model, param.mobilenet_config, param.mobilenet_class, 
                            input_files, INPUT_WIDTH, INPUT_HEIGHT);
    } 
    else {
        LOG_ERROR("Main:    Incorrect Model name!");
        return -1;
    }
    traffic.detection_time = time.stop();

    // Find Vehicles Trajectory:
    LOG_INFO("Main:          Tracking the vehicles:");
    Lane lines(param.data_path + "lines.txt");
    time.start();
    std::vector<Centroid> tracked_vehicles;
    cv::Size image_size = {INPUT_WIDTH, INPUT_HEIGHT};
    VehicleTracker tracker;
    tracker.track(vector_of_vehicles, tracked_vehicles, lines, image_size);
    traffic.tracking_time = time.stop();
    LOG_TRACE("Main:          Tracked number of vehicle: ", tracked_vehicles.size() );

    

    // Traffic estimation:
    LOG_INFO("Main:          Estimating the traffic:");
    time.start();
    TrafficEstimator estimator;
    traffic = estimator.estimate(tracked_vehicles, lines, image_size);
    traffic.estimation_time = time.stop();
    LOG_TRACE("Main:          Estimated Traffic is: ", traffic.prediction );

    std::cout << "Computation Time for " << input_files.size() << " frames:\n";
    std::cout << "    -Detection     = " << std::fixed << std::setprecision(4)<< traffic.detection_time << "s\n";
    std::cout << "    -Tracking      = " << std::fixed << std::setprecision(4)<< traffic.tracking_time << "s\n";
    std::cout << "    -Estimation    = " << std::fixed << std::setprecision(4)<< traffic.estimation_time << "s\n";
    std::cout << "    -Total         = " << std::fixed << std::setprecision(4)<< total_time.stop() << "s\n";

    // Write to result file:
    FileUtils::save(tracked_vehicles, traffic);
    FileUtils::drawResultOnImage(input_files, lines, vector_of_vehicles, INPUT_WIDTH, INPUT_HEIGHT);

    return 0;
}

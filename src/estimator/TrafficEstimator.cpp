#include "TrafficEstimator.hpp"


void TrafficEstimator::estimate(const std::vector<Centroid>& vehicles,
                                     const Lane& lines, const cv::Size& image_size, Result& prediction){
	
	// Calculate average traffic flow speed:
    prediction.average_flow_speed = [&]() {
        float avg = 0;
        int count = 0;
        for(auto it = vehicles.begin(); it != vehicles.end(); it++){
            if(it->position_history.size() > 1){
                avg += it->speed;
                count++;
            }
        }
        return (avg/count);
    }();

    // Find the lane with the highest flow:
    prediction.highest_lane_flow = [&]() {
        std::map<int, int> frequency_map;
        int max_frequency = 0;
        int most_frequent_element = 0;
        for(auto it = vehicles.begin(); it != vehicles.end(); it++){
            int value = ++frequency_map[it->lane_num];
            if (value > max_frequency)
            {
                max_frequency = value;
                most_frequent_element = it->lane_num;
            }
        }
        return most_frequent_element;
    }();

    // Density estimation: 
    prediction.density = densityEstimator(vehicles, lines, image_size);

    // Traffci estimation:
    std::stringstream result;

    cv::Ptr<cv::ml::SVM> svm = cv::ml::SVM::load("../models/svm/svm_model.xml");
    LOG_TRACE("Estimator: Svm model is loaded from: ", "../models/svm/svm_model.xml");

    float input[1][3] = {static_cast<float>(vehicles.size()), prediction.density, prediction.average_flow_speed};
    cv::Mat inputMat(1, 3, CV_32F, input);
    float response = svm->predict(inputMat);
    if (response == 1)
        result << "Low";
    else if (response == 2)
        result << "Medium";
    else if (response == 3)
        result << "High";
    prediction.prediction = result.str();
}

float TrafficEstimator::densityEstimator(const std::vector<Centroid>& vehicles, const Lane& lines, const cv::Size& image_size){

    int a = (lines.line.front().first.x - lines.line.front().second.x);
    int b = (lines.line.back().second.x - lines.line.back().first.x);
    int h = (image_size.height - lines.line.front().first.y);

    // area of a trapezoid: A = (a + b) × h / 2
    float trapezoid_area = ((a+b) * h) / 2;
    float sum_of_boxes_area = [&]() {
        int sum_of_elems =0;
        for(auto it = vehicles.begin(); it != vehicles.end(); it++){
            sum_of_elems += it->area;
        }
        return sum_of_elems;
    }();
    return (sum_of_boxes_area/trapezoid_area) * 100;
}

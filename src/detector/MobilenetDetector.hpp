#pragma once
#include <vector>
#include <random>
#include <iostream>
#include <string>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/all_layers.hpp>
#include "../entity/Vehicle.hpp"

class MobilenetDetector
{
public:
	void loadModel(const std::string& model_path,
					   const std::string& model_config,
					   	   const std::string& classes_path);

	void predict(std::vector<std::vector<Centroid>>& vector_of_centeroids, std::vector<std::string>& input_files, 
					 const float INPUT_WIDTH, const float INPUT_HEIGHT);
private:

    const float SCORE_THRESHOLD = 0.2;
    const float NMS_THRESHOLD = 0.5;
    const float CONFIDENCE_THRESHOLD = 0.5;

	cv::dnn::Net model;
    std::vector<std::string> classes;
};
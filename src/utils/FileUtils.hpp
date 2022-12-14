#pragma once
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <random>
#include <iomanip>

#include "Timer.hpp"
#include "Lane.hpp"
#include "../entity/Vehicle.hpp"
#include "../entity/Result.hpp"
#include <opencv2/core/types.hpp>
#include <opencv2/imgcodecs.hpp>


class FileUtils {
public:
   static void save(std::vector<Centroid>& tracked_vehicles, const Result& result);
   static void drawResultOnImage(std::vector<std::string>& input_files, Lane& lines,
                                    std::vector<std::vector<Centroid>>& vector_of_centeroids, 
                                       const std::string images[],
                                          const float INPUT_WIDTH, const float INPUT_HEIGHT);
private:
   static void drawGrid(cv::Mat &image, Lane& lines_vector);
};
#pragma once
#include <string>

struct Result {
    float average_flow_speed = 0.0f;
    int highest_lane_flow = 0;
    float density = 0.0f;
    std::string prediction{""};

    float detection_time = 0.0f;
    float tracking_time = 0.0f;
    float estimation_time = 0.0f;
};
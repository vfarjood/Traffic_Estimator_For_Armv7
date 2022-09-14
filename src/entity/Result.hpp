#pragma once
#include <string>

struct Result {
    float average_flow_speed;
    int highest_lane_flow;
    float density;
    std::string prediction;
};
#include "VehicleTracker.hpp"

void VehicleTracker::track(std::vector<std::vector<Centroid>>& vector_of_centeroids, 
                        std::vector<Centroid>& tracking_vehicles, Lane& lines, const cv::Size& image_size )
{
    bool first_image = true;
    for(auto & current_vehicles: vector_of_centeroids)
    {
        if (first_image == true) 
        {
            for (auto &vehicle : current_vehicles) 
            {
                vehicle.position_history.push_back(vehicle.center);
                tracking_vehicles.push_back(vehicle);
            }
            lines.findLineNumber(tracking_vehicles, image_size);
            first_image = false;
        } 
        else 
        {
            findVehiclesTrajectory(tracking_vehicles, current_vehicles, lines, image_size);
        }
    }
}

void VehicleTracker::findVehiclesTrajectory(std::vector<Centroid> &existingVehicles,
                                std::vector<Centroid> &currentFrameVehicles,
                                    Lane& lines, const cv::Size& image_size) 
{
    lines.findLineNumber(currentFrameVehicles, image_size);

    for (auto &currentFrameVehicle : currentFrameVehicles) 
    {
        int least_distance_index = 0;
        double least_distance = 100000.0;
        for (int i = 0; i < existingVehicles.size(); i++) 
        {
            if((existingVehicles[i].lane_num == currentFrameVehicle.lane_num) &&
               (existingVehicles[i].position_history.size() == 1))
            {
                if ((existingVehicles[i].area >= currentFrameVehicle.area) ||
                    (existingVehicles[i].box.y >= currentFrameVehicle.box.y))
                {
                    double distance = distanceBetweenPoints(currentFrameVehicle.center, existingVehicles[i].center);
                    if (distance < least_distance) 
                    {
                        least_distance = distance;
                        least_distance_index = i;
                    }
                }
            }
        }

        if (least_distance < image_size.height){
            const int fps = 1;
            const float speed_factor = (existingVehicles[least_distance_index].center.y / image_size.height) + 2;
            existingVehicles[least_distance_index].distance = least_distance;
            existingVehicles[least_distance_index].speed = (least_distance/speed_factor)/fps;
            existingVehicles[least_distance_index].box_history.push_back(currentFrameVehicle.box);
            existingVehicles[least_distance_index].position_history.push_back(currentFrameVehicle.center);
        } else { 
            currentFrameVehicle.position_history.push_back(currentFrameVehicle.center);
            existingVehicles.push_back(currentFrameVehicle);
        }
    }
}

double VehicleTracker::distanceBetweenPoints(const cv::Point& point1, const cv::Point& point2) 
{
    double distance = sqrt(pow((point2.x - point1.x), 2) + pow((point2.y - point1.y), 2));
    return distance;
}
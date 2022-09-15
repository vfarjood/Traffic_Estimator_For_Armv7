#include "FileUtils.hpp"


void FileUtils::save(std::vector<Centroid>& tracked_vehicles, const Result& result)
{
    std::ofstream result_file("../result/result.txt");
    if (!result_file.is_open()){
        std::cout << "Unable to open file";
    }
    // sort the vecotr of vehicle based on their lane number:
    std::sort(tracked_vehicles.begin(), tracked_vehicles.end(), 
            [](const Centroid& v1, const Centroid& v2) 
                {return v1.lane_num < v2.lane_num;});
    Timer current;
    result_file << "*** "<< current.getCurrentTime() << " ***\n\n";
    result_file << "Traffic Status: " << result.prediction << "\n";
    result_file << "Number of cars: "  << tracked_vehicles.size() << "\n";
    result_file << "Flow density: "  << (int)result.density << "% \n";
    result_file << "Flow average speed: "  << (int)result.average_flow_speed << "\n";
    result_file << "Highest flow is in lane number: "  << result.highest_lane_flow << "\n";
    result_file << "---------------------" << "\n\n";

    // write quantitative information into results file:
    for(int num=0; num < tracked_vehicles.size(); num++)
    {
        result_file << "Vehicle Number: "<< (num+1) << "\n";
        result_file << "Vehicle Type: "  << tracked_vehicles[num].name << "\n";
        result_file << "Lane Number: "   << tracked_vehicles[num].lane_num << "\n";
        result_file << "Speed: "         << tracked_vehicles[num].speed << "\n";
        result_file << "Positions: "     << "\n";
        for(int i = 0; i < tracked_vehicles[num].position_history.size(); i++){
            result_file << "    frame["<< (int)(i+1) << "]: [" <<
            tracked_vehicles[num].position_history[i].x << ", " <<
            tracked_vehicles[num].position_history[i].y << "]\n";
        }
        result_file << "---------------------" << "\n";
    }
}

void FileUtils::drawResultOnImage(std::vector<std::string>& input_files, Lane& lines,
                         std::vector<std::vector<Centroid>>& vector_of_centeroids, const std::string images[],
                            const float INPUT_WIDTH, const float INPUT_HEIGHT)
{
    // std::default_random_engine generator;
    std::mt19937 generator(2019);
    // set uniform distribution for each R,G,B color:
    std::uniform_int_distribution<int> distribution(0, 255);

    int j=0;
    for(auto const& centroids: vector_of_centeroids)
    {    
        cv::Mat image = cv::imread(input_files[j], 1);
        std::string path =  "../result/" + images[j];

        cv::resize(image, image, cv::Size(INPUT_WIDTH, INPUT_HEIGHT));
        for(int i = 0; i < centroids.size(); i++)
        {
            cv::Scalar color = cv::Scalar(0.0, 255.0, 255.0);
            color = cv::Scalar(distribution(generator), distribution(generator), distribution(generator));
            std::string label = "[" + centroids[i].name + " : " + cv::format("%.2f",centroids[i].conf) + "]"; // + " L:" + std::to_string(centroids[i].lane_num);
    
            cv::rectangle(image, cv::Point(centroids[i].box.x, centroids[i].box.y), cv::Point(centroids[i].box.x+centroids[i].box.width, centroids[i].box.y+centroids[i].box.height), color, 2);
            cv::putText(image, label, cv::Point(centroids[i].box.x, centroids[i].box.y-5), cv::FONT_HERSHEY_SIMPLEX, 0.5, color, 1);
        }
        lines.draw(image);
        cv::imwrite(path, image);
        ++j;
    }
}
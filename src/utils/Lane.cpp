#include "Lane.hpp"


Lane::Lane(const std::string& filename){
    std::vector<int> numbers;
    int top_width, top_height, down_width, down_height;

    std::ifstream input_file(filename);
    if (!input_file.is_open()) {
        std::cout << "Could not open the file - '"
             << filename << "'" << std::endl;
    }
    while (input_file >> top_width >> top_height >> down_width >> down_height) {
        line.emplace_back(std::make_pair(cv::Point(top_width, top_height), cv::Point(down_width ,down_height)));
    }
    input_file.close();
}

void Lane::draw(cv::Mat const& image){
    for(int i = 0; i < line.size(); i++){
        cv::line(image, line[i].first, line[i].second, cv::Scalar(173.0, 255.0, 47.0), 2);
    }
}

void Lane::findLineNumber(std::vector<Centroid> &vehicles, const cv::Size& image_size) 
{
    for (auto &vehicle : vehicles) 
    {
        for(int i = 0; i < line.size(); i++)
        {
            if(checkTheLine(vehicle.center, line[i], line[i+1], image_size))
            {
                vehicle.lane_num = i+1;
            }
        }
    }
}

bool Lane::checkTheLine(const cv::Point& center, 
                    const std::pair<cv::Point, cv::Point>& left_line, 
                        const std::pair<cv::Point, cv::Point>& right_line,
                            const cv::Size& image_size){
    bool left  = false;
    bool right = false;
    cv::LineIterator left_it(image_size,left_line.first, left_line.second, 8);
    cv::LineIterator rigth_it(image_size,right_line.first, right_line.second, 8);
    for(int i = 0; i < left_it.count; i++, ++left_it)
    {
        cv::Point pt= left_it.pos();
        if((center.y == pt.y) && (center.x >= pt.x))
            left = true;
    }
    for(int i = 0; i < rigth_it.count; i++, ++rigth_it)
    {
        cv::Point pt= rigth_it.pos();
        if((center.y == pt.y) && (center.x <= pt.x))
            right = true;
    }
    if(left && right)
        return true;
    else
        return false;
}

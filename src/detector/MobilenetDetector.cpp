#include "MobilenetDetector.hpp"

void MobilenetDetector::loadModel(const std::string& model_path,
									const std::string& model_config,
					 					const std::string& classes_path) {
    
    // load yolo neural network model:
    model = cv::dnn::readNetFromTensorflow(model_path, model_config);

    // Load class names:
    classes.reserve(5);
    std::ifstream ifs(std::string(classes_path).c_str());
    std::string line;
    while (getline(ifs, line)) {classes.push_back(line);} 
        
}

void MobilenetDetector::predict(std::vector<std::vector<Centroid>>& vector_of_centeroids,
								    std::vector<std::string>& input_files, 
										const float INPUT_WIDTH, const float INPUT_HEIGHT){

	for (std::string const& file : input_files)
    {
        // Load the image:
        cv::Mat image = cv::imread(file, 1);
        cv::resize (image, image, cv::Size(INPUT_WIDTH, INPUT_HEIGHT));
        
        // create blob from image:
        cv::Mat input = cv::dnn::blobFromImage(image, 1.0, cv::Size(INPUT_WIDTH, INPUT_HEIGHT), cv::Scalar(127.5, 127.5, 127.5), true, false);
    
        // set the blob to the model:
        model.setInput(input);
    
        // forward pass through the model to carry out the detection:
        cv::Mat output = model.forward();
        
        std::vector<Centroid> centroids;
        centroids.reserve(5);

        cv::Mat detectionMat(output.size[2], output.size[3], CV_32F, output.ptr<float>());
        for (int i = 0; i < detectionMat.rows; i++){

            float conf = detectionMat.at<float>(i, 2);
    
            // Check if the detection is of good quality:
            if (conf > CONFIDENCE_THRESHOLD)
            {
                Centroid object;
    
                object.id         = detectionMat.at<float>(i, 1);
                object.name       = classes[object.id-1];
                object.conf       = detectionMat.at<float>(i, 2);
                object.box.x      = static_cast<int>(detectionMat.at<float>(i, 3) * image.cols);
                object.box.y      = static_cast<int>(detectionMat.at<float>(i, 4) * image.rows);
                object.box.width  = static_cast<int>(detectionMat.at<float>(i, 5) * image.cols - object.box.x);
                object.box.height = static_cast<int>(detectionMat.at<float>(i, 6) * image.rows - object.box.y);
                object.area = (object.box.width * object.box.height);
                object.center     = cv::Point((object.box.x + (object.box.width/2)), (object.box.y + (object.box.height/2)));
                centroids.push_back(object);
            }
        }
        vector_of_centeroids.push_back(centroids);
    }
}
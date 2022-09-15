#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/all_layers.hpp>
#include <opencv2/ml.hpp>


int main(int argc, char** argv)
{
  size_t size = 60;

  // Set up training data
  int labels[size] = {3, 1, 2, 3, 3, 2,
                      2, 2, 2, 1, 3, 3,
                      3, 1, 1, 2, 1, 1,
                      1, 3, 2, 2, 3, 3,
                      3, 1, 1, 2, 1, 1,
                      3, 1, 2, 2, 3, 3,
                      1, 1, 2, 3, 2, 2,
                      1, 1, 2, 3, 2, 2,
                      1, 2, 3, 3, 3, 1,
                      2, 1, 3, 2, 3, 1 };

  float trainingData[size][3] = { {25, 120, 8}, {6, 2, 12},  {12, 31, 26}, {21, 105, 2}, {15, 75, 12},{6, 54, 33},
                                  {10, 27, 46}, {9, 18, 31}, {10, 31, 25}, {8, 16, 18},  {17, 87, 15},{10, 78, 9},
                                  {16, 81, 9},  {6, 10, 20}, {6, 9, 26},   {12, 48, 38}, {10, 20, 66},{1, 9, 90}, 
                                  {13, 25, 78}, {20, 88, 15},{7, 50, 35},  {8, 60, 40},  {19, 110, 6},{15, 68, 17},
                                  {16, 90, 4},  {11, 30, 90},{3, 19, 80},  {14, 75, 45}, {7, 28, 55}, {3, 31, 74}, 
                                  {20, 100, 1}, {5, 18, 81}, {11, 65, 38}, {17, 50, 63}, {10, 99, 3}, {18, 97, 11},
                                  {9, 32, 76},  {8, 24, 61}, {4, 59, 32},  {18, 93, 14}, {15, 75, 48},{18, 44, 50},  
                                  {1, 11, 100}, {2, 13, 37}, {20, 55, 50}, {11, 77, 5},  {6, 67, 41}, {14, 39, 66},
                                  {3, 22, 49},  {13, 58, 58},{5, 89, 8},   {8, 79, 11},  {13, 85, 20},{3, 16, 72},
                                  {4, 40, 49},  {8, 34, 68}, {15, 89, 1},  {17, 35, 48}, {28, 95, 2}, {6, 23, 73} };

  cv::Mat trainingDataMat(size, 3, CV_32F, trainingData);
  cv::Mat labelsMat(size, 1, CV_32SC1, labels);

    // Train the SVM
    cv::Ptr<cv::ml::SVM> svm = cv::ml::SVM::create();
    svm->setType(cv::ml::SVM::C_SVC);
    svm->setKernel(cv::ml::SVM::LINEAR);
    svm->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, 100, 1e-6));
    svm->train(trainingDataMat, cv::ml::ROW_SAMPLE, labelsMat);
    svm->save("../../svm_model.xml");

    return 0;
}
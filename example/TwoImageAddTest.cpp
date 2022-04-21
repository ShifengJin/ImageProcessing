#include <iostream>
#include <string>
#include "TwoImageAdd.h"
#include <opencv2/opencv.hpp>

int main(int argc, char* argv[]){

    if(argc != 5){
        std::cout << "./TwoImageAddTest image1_path image2_path out_image_path weight" << std::endl;
        return 0;
    }
    std::string image1Path = argv[1];
    std::string image2Path = argv[2];
    std::string outImagePath = argv[3];
    float weight = atof(argv[4]);
    cv::Mat image1 = cv::imread(image1Path);
    cv::Mat image2 = cv::imread(image2Path);

    if(image1.rows != image2.rows || image1.cols != image2.cols){
        std::cout << "image1 and image2 size must same." << std::endl;
        return 0;
    }

    cv::Mat outImage(image1.rows, image1.cols, CV_8UC3);
    TwoImageAdd(image1.data, image2.data, outImage.data, image1.cols, image1.rows, weight);

    cv::imwrite(outImagePath, outImage);

    return 0;
}
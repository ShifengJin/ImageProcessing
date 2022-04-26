#include <iostream>
#include <vector>
#include <string>
#include "ImageBlur.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main(int argc, char* argv[]){

    if(argc < 5){
        std::cout << "./ImageBlurTest image_path out_image_path type ksize otherParam" << std::endl;
        return 0;
    }

    std::string imagePath = argv[1];
    std::string outImagePath = argv[2];
    BLURTYPE blurType = (BLURTYPE)atoi(argv[3]);
    int ksize = atoi(argv[4]);
    
    int width, height, channels;
    stbi_uc* image = stbi_load(imagePath.c_str(), &width, &height, &channels, STBI_rgb);
    if(image == NULL){
        std::cout << "load " << imagePath << " error." << std::endl;
        return 0;
    }

    unsigned char* oImageBuffer = (unsigned char*)calloc(width * height * channels, sizeof(unsigned char));

    if(oImageBuffer){

        switch(blurType){
            case GAUSS:
            {
                std::vector<float> filter(ksize * ksize, 0.f);
                float sigma = atof(argv[5]);
                GaussFilter(ksize, sigma, filter.data());
                ImageConvolution(image, width, height, channels, oImageBuffer, filter.data(), ksize);
            }
            break;
            case MOTION:
            {
                std::vector<float> filter(ksize * ksize, 0.f);
                float angle = atof(argv[5]);
                MotionFilter(ksize, angle, filter.data());
                ImageConvolution(image, width, height, channels, oImageBuffer, filter.data(), ksize);
            }
            break;
            case MEDIAN:
            {
                ImageMedian(image, width, height, channels, oImageBuffer,  ksize);
            }
            break;
            case AVG:
            {
                ImageAvg(image, width, height, channels, oImageBuffer, ksize);
            }
            break;
            case GLASS:
            {
                ImageGlass(image, width, height, channels, oImageBuffer, ksize);
            }
            break;
            default:
            break;
        }
        stbi_write_jpg(outImagePath.c_str(), width, height, STBI_rgb, oImageBuffer, 100);
        free(oImageBuffer);
        stbi_image_free(image);
    }else{
        std::cout << "calloc " << width * height * channels * sizeof(unsigned char) << " size buffer error." << std::endl;
        stbi_image_free(image);
    }

    return 0;
}
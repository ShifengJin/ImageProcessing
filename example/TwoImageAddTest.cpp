#include <iostream>
#include <string>
#include "TwoImageAdd.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main(int argc, char* argv[]){

    if(argc != 5){
        std::cout << "./TwoImageAddTest image1_path image2_path out_image_path weight" << std::endl;
        return 0;
    }
    std::string image1Path = argv[1];
    std::string image2Path = argv[2];
    std::string outImagePath = argv[3];
    float weight = atof(argv[4]);

    int image1Width, image1Height, image1Channels;
    stbi_uc* image1 = stbi_load(image1Path.c_str(), &image1Width, &image1Height, &image1Channels, STBI_rgb);
    if(image1 == NULL){
        std::cout << "load " << image1Path << " error." << std::endl;
        return 0;
    }

    int image2Width, image2Height, image2Channels;
    stbi_uc* image2 = stbi_load(image2Path.c_str(), &image2Width, &image2Height, &image2Channels, STBI_rgb);
    if(image2 == NULL){
        std::cout << "load " << image2Path << " error." << std::endl;
        stbi_image_free(image1);
        return 0;
    }

    if(image1Width != image2Width || image1Height != image2Height){
        std::cout << "image1 and image2 size must same." << std::endl;
        stbi_image_free(image1);
        stbi_image_free(image2);
        return 0;
    }
    
    unsigned char* oImageBuffer = (unsigned char*)calloc(image1Width * image1Height * image1Channels, sizeof(unsigned char));

    if(oImageBuffer){
        TwoImageAdd(image1, image2, oImageBuffer, image1Width, image1Height, weight);
        stbi_write_jpg(outImagePath.c_str(), image1Width, image1Height, STBI_rgb, oImageBuffer, 100);
        free(oImageBuffer);
        stbi_image_free(image1);
        stbi_image_free(image2);
    }else{
        std::cout << "calloc " << image1Width * image1Height * image1Channels * sizeof(unsigned char) << " size buffer error." << std::endl;
        stbi_image_free(image1);
        stbi_image_free(image2);
    }

    return 0;
}
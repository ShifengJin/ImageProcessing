#include <iostream>
#include <string>
#include "ImageRotate.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main(int argc, char* argv[]){

    if(argc < 4){
        std::cout << "./ImageRotateTest image_path out_image_path angle" << std::endl;
        return 0;
    }

    std::string imagePath = argv[1];
    std::string outImagePath = argv[2];
    float angle = atof(argv[3]);
    
    int width, height, channels;
    stbi_uc* image = stbi_load(imagePath.c_str(), &width, &height, &channels, STBI_rgb);
    if(image == NULL){
        std::cout << "load " << imagePath << " error." << std::endl;
        return 0;
    }

    unsigned char* oImageBuffer = (unsigned char*)calloc(width * height * channels, sizeof(unsigned char));

    if(oImageBuffer){
        ImageRotate(image, width, height, channels, oImageBuffer, angle);
        stbi_write_jpg(outImagePath.c_str(), width, height, STBI_rgb, oImageBuffer, 100);
        free(oImageBuffer);
        stbi_image_free(image);
    }else{
        std::cout << "calloc " << width * height * channels * sizeof(unsigned char) << " size buffer error." << std::endl;
        stbi_image_free(image);
    }

    return 0;
}
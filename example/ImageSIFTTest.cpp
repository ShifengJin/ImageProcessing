#include <iostream>
#include <vector>
#include <string>
#include "ImageTransformation.h"
#include "SIFT.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main(int argc, char* argv[]){

    if(argc < 2){
        std::cout << "./ImageSIFTTest image_path" << std::endl;
        return 0;
    }

    std::string imagePath = argv[1];
    
    int width, height, channels;
    stbi_uc* image = stbi_load(imagePath.c_str(), &width, &height, &channels, STBI_rgb);
    if(image == NULL){
        std::cout << "load " << imagePath << " error." << std::endl;
        return 0;
    }

    float* grayImageBuffer = (float*)calloc(width * height, sizeof(float));
    Utily::RGB2Gray(image, grayImageBuffer, width, height);

    SIFT::ptr pSIFT = SIFT::ptr(new SIFT((unsigned int)width, (unsigned int)height));
    pSIFT->Run(grayImageBuffer);
    pSIFT.reset();

    stbi_image_free(image);
    FREE_Memory(grayImageBuffer);

    return 0;
}
#include <iostream>
#include <string>
#include "ImageInterpolation.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main(int argc, char* argv[]){

    if(argc != 5){
        std::cout << "./ImageInterpolationTest image_path out_image_path scale type" << std::endl;
        return 0;
    }
    std::string imagePath = argv[1];
    std::string outImagePath = argv[2];
    float scale = atof(argv[3]);
    INTERPOLATIONTYPE type = (INTERPOLATIONTYPE)atoi(argv[4]);

    int width, height, channels;
    stbi_uc* image = stbi_load(imagePath.c_str(), &width, &height, &channels, STBI_rgb);
    if(image == NULL){
        std::cout << "load " << imagePath << " error." << std::endl;
        return 0;
    }
    int oWidth, oHeight;
    unsigned char* oBuffer = NULL;
    ImageInterpolation_Init(width, height, channels, scale, &oWidth, &oHeight, &oBuffer);
    if(oWidth < 10 || oHeight < 10 || oBuffer == NULL){
        std::cout << "ImageInterpolation Init error. oWidth: " << oWidth << "  oHeight: " << oHeight << "  or oBuffer is NULL." << std::endl;
        stbi_image_free(image);
        return 0;
    }

    switch(type){
        case NEAREST:
        {
            ImageInterpolation_Nearest(image, width, height, channels, oBuffer, oWidth, oHeight);
        }
        break;
        case LINEAR:
        {
            ImageInterpolation_Linear(image, width, height, channels, oBuffer, oWidth, oHeight);
        }
        break;
        default:
        break;
    }

    stbi_write_jpg(outImagePath.c_str(), oWidth, oHeight, STBI_rgb, oBuffer, 100);
    
    ImageInterpolation_Unit(&oBuffer);
    stbi_image_free(image);

    return 0;
}
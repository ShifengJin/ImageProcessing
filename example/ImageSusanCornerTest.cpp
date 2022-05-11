#include <iostream>
#include <string>
#include "Common.h"
#include "ImageTransformation.h"
#include "ImageSusanCorner.h"
#include "ImageBlur.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main(int argc, char* argv[]){

    if(argc < 3){
        std::cout << "./ImageHarrisCornerTest image_path out_image_path" << std::endl;
        return 0;
    }

    std::string imagePath = argv[1];
    std::string outImagePath = argv[2];
    
    int width, height, channels;
    stbi_uc* image = stbi_load(imagePath.c_str(), &width, &height, &channels, STBI_rgb);
    if(image == NULL){
        std::cout << "load " << imagePath << " error." << std::endl;
        return 0;
    }

    float* rgb = (float*)calloc(width * height * channels, sizeof(float));
    float* gray = (float*)calloc(width * height, sizeof(float));
    float* R = (float*)calloc(width * height, sizeof(float));    

    Normalization<unsigned char, float>(image, rgb, width * height * channels, 0.f, 1.f);
    
    RGB2Gray(rgb, gray, width, height);
    std::vector<Coordinate_i> CornerCoordiantes;
    int maskRadius = 3;
    int unSuppressionRadius = 3;
    float threshold = 50.f / 255.f;
    SusanCorner(gray, width, height, CornerCoordiantes, maskRadius, unSuppressionRadius, threshold, R);

    DrawPointToImageRGB(image, image, width, height, {(unsigned char)0, (unsigned char)0, (unsigned char)0}, CornerCoordiantes);

    stbi_write_jpg(outImagePath.c_str(), width, height, STBI_rgb, image, 100);
    stbi_image_free(image);

    FREE_Memory(rgb);
    FREE_Memory(gray);
    FREE_Memory(R);

    return 0;
}
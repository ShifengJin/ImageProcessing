#include <iostream>
#include <string>
#include "Common.h"
#include "ImageTransformation.h"
#include "ImageHarrisCorner.h"
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
    float* Ix2 = (float*)calloc(width * height, sizeof(float));
    float* Iy2 = (float*)calloc(width * height, sizeof(float));
    float* IxIy = (float*)calloc(width * height, sizeof(float));
    float* Gauss_Ix2 = (float*)calloc(width * height, sizeof(float));
    float* Gauss_Iy2 = (float*)calloc(width * height, sizeof(float));
    float* Gauss_IxIy = (float*)calloc(width * height, sizeof(float));
    float* MR = (float*)calloc(width * height, sizeof(float));

    Utily::Normalize<unsigned char, float>(image, rgb, width * height * channels, 0.f, 1.f);
    
    Utily::RGB2Gray(rgb, gray, width, height);
#if 0
    {
        unsigned char* tmp = (unsigned char*)calloc(width * height, sizeof(unsigned char));
        Utily::Normalize<float, unsigned char>(gray, tmp, width * height, 0, 255);

        stbi_write_jpg("gray.jpg", width, height, STBI_grey, tmp, 100);
        FREE_Memory(tmp);
    }
#endif
    float k = 0.04f;
    int ksize = 3;
    float sigma = 1.5f;
    float* GaussKernel = (float*)calloc(ksize * ksize, sizeof(float));
    GaussFilter(ksize, sigma, GaussKernel);
    std::vector<Coordinate_i> CornerCoordiantes;

    HarrisCorner(gray, width, height, CornerCoordiantes, k, ksize, GaussKernel, Ix2, Iy2, IxIy, Gauss_Ix2, Gauss_Iy2, Gauss_IxIy, MR, 5);
    
#if 0
    {
        unsigned char* tmp = (unsigned char*)calloc(width * height, sizeof(unsigned char));
        Utily::Normalize<float, unsigned char>(Ix2, tmp, width * height, 0, 255);
        DrawPointToImageGray(tmp, tmp, width, height, (unsigned char)255, CornerCoordiantes);
        stbi_write_jpg("Ix2.jpg", width, height, STBI_grey, tmp, 100);

        Utily::Normalize<float, unsigned char>(Iy2, tmp, width * height, 0, 255);
        DrawPointToImageGray(tmp, tmp, width, height, (unsigned char)255, CornerCoordiantes);
        stbi_write_jpg("Iy2.jpg", width, height, STBI_grey, tmp, 100);

        Utily::Normalize<float, unsigned char>(IxIy, tmp, width * height, 0, 255);
        DrawPointToImageGray(tmp, tmp, width, height, (unsigned char)255, CornerCoordiantes);
        stbi_write_jpg("IxIy.jpg", width, height, STBI_grey, tmp, 100);
        
        Utily::Normalize<float, unsigned char>(Gauss_Ix2, tmp, width * height, 0, 255);
        DrawPointToImageGray(tmp, tmp, width, height, (unsigned char)255, CornerCoordiantes);
        stbi_write_jpg("Gauss_Ix2.jpg", width, height, STBI_grey, tmp, 100);

        Utily::Normalize<float, unsigned char>(Gauss_Iy2, tmp, width * height, 0, 255);
        DrawPointToImageGray(tmp, tmp, width, height, (unsigned char)255, CornerCoordiantes);
        stbi_write_jpg("Gauss_Iy2.jpg", width, height, STBI_grey, tmp, 100);

        Utily::Normalize<float, unsigned char>(Gauss_IxIy, tmp, width * height, 0, 255);
        DrawPointToImageGray(tmp, tmp, width, height, (unsigned char)255, CornerCoordiantes);
        stbi_write_jpg("Gauss_IxIy.jpg", width, height, STBI_grey, tmp, 100);

        FREE_Memory(tmp);
    }
#endif


    DrawPointToImageRGB(image, image, width, height, {(unsigned char)0, (unsigned char)0, (unsigned char)0}, CornerCoordiantes);

    stbi_write_jpg(outImagePath.c_str(), width, height, STBI_rgb, image, 100);
    stbi_image_free(image);

    FREE_Memory(rgb);
    FREE_Memory(gray);
    FREE_Memory(Ix2);
    FREE_Memory(Iy2);
    FREE_Memory(IxIy);
    FREE_Memory(Gauss_Ix2);
    FREE_Memory(Gauss_Iy2);
    FREE_Memory(Gauss_IxIy);
    FREE_Memory(MR);

    return 0;
}
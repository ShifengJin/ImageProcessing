#include <iostream>
#include <vector>
#include <string>
#include "ImageEdge.h"
#include "ImageTransformation.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main(int argc, char* argv[]){

    if(argc < 5){
        std::cout << "./ImageEdgeTest image_path out_image_path EdgeType MagType" << std::endl;
        std::cout << "EdgeType: " << std::endl;
        std::cout << "    EDGE_SOBEL = 0" << std::endl;
        std::cout << "    EDGE_ROBERTS = 1" << std::endl;
        std::cout << "    EDGE_PREWITT = 2" << std::endl;
        std::cout << "    EDGE_LAPLACIAN = 3" << std::endl;
        std::cout << "    EDGE_CANNY = 4" << std::endl;
        std::cout << "=====================================" << std::endl;
        std::cout << "MagType: (EdgeType={EDGE_SOBEL, EDGE_ROBERTS, EDGE_PREWITT})" << std::endl;
        std::cout << "    MAG_L1 = 0" << std::endl;
        std::cout << "    MAG_L2 = 1" << std::endl;
        std::cout << "    MAG_LInfinite = 2" << std::endl;
        std::cout << "MagType: (EdgeType={EDGE_LAPLACIAN})" << std::endl;
        std::cout << "    X_0_Y_0 = 0" << std::endl;
        std::cout << "    X_45_Y_45 = 1" << std::endl;
        std::cout << "    X_0_45_Y_0_45 = 2" << std::endl;
        return 0;
    }

    std::string imagePath = argv[1];
    std::string outImagePath = argv[2];
    EDGETYPE edgeType = (EDGETYPE)atoi(argv[3]);
    MAGTYPE magType = (MAGTYPE)atoi(argv[4]);
    
    int width, height, channels;
    stbi_uc* image = stbi_load(imagePath.c_str(), &width, &height, &channels, STBI_rgb);
    if(image == NULL){
        std::cout << "load " << imagePath << " error." << std::endl;
        return 0;
    }

    unsigned char* oImageGray = (unsigned char*)calloc(width * height, sizeof(unsigned char));
    float* imageGrayFloat = (float*)calloc(width * height, sizeof(float));
    float* imageEdgefloat = (float*)calloc(width * height, sizeof(float));
    if(oImageGray && imageGrayFloat && imageEdgefloat){
        Utily::RGB2Gray(image, imageGrayFloat, width, height);
        

        switch(edgeType){
            case EDGE_SOBEL:
            {
                Sobel::ptr pSobel = Sobel::ptr(new Sobel(width, height));
                pSobel->Run(imageGrayFloat, imageEdgefloat, magType);
                pSobel.reset();
            }
            break;
            case EDGE_ROBERTS:
            {
                Roberts::ptr pRoberts = Roberts::ptr(new Roberts(width, height));
                pRoberts->Run(imageGrayFloat, imageEdgefloat, magType);
                pRoberts.reset();
            }
            break;
            case EDGE_PREWITT:
            {
                Prewitt::ptr pPrewitt = Prewitt::ptr(new Prewitt(width, height));
                pPrewitt->Run(imageGrayFloat, imageEdgefloat, magType);
                pPrewitt.reset();
            }
            break;
            case EDGE_LAPLACIAN:
            {
                Laplacian::ptr pLaplacian = Laplacian::ptr(new Laplacian(width, height));
                pLaplacian->Run(imageGrayFloat, imageEdgefloat, (Laplacian::Type)magType);
                pLaplacian.reset();
            }
            break;
            case EDGE_CANNY:
            {
                break;
            }
            break;
            default:
            break;
        }
        
        Utily::Normalize<float, unsigned char>(imageEdgefloat, oImageGray, width * height, (unsigned char)0, (unsigned char)255);

        stbi_write_jpg(outImagePath.c_str(), width, height, STBI_grey, oImageGray, 100);
        FREE_Memory(oImageGray);
        FREE_Memory(imageGrayFloat);
        FREE_Memory(imageEdgefloat);
        stbi_image_free(image);
    }else{
        std::cout << "calloc " << width * height * channels * sizeof(unsigned char) << " size buffer error." << std::endl;
        stbi_image_free(image);
    }

    return 0;
}
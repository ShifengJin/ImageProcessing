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

    if(argc < 3){
        std::cout << "./ImageSIFTTest image_path outImg_path" << std::endl;
        return 0;
    }

    std::string imagePath = argv[1];
    std::string outImgPath = argv[2];
    
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

    std::vector<Vector2i> CornerCoordiantes;
    CornerCoordiantes.resize(pSIFT->m_Coordiantes.size());
    for(unsigned int i = 0; i < pSIFT->m_Coordiantes.size(); ++ i){
        CornerCoordiantes[i].x = (int)roundf32(pSIFT->m_Coordiantes[i].m_subCoordiante.x);
        CornerCoordiantes[i].y = (int)roundf32(pSIFT->m_Coordiantes[i].m_subCoordiante.y);
    }
    pSIFT.reset();

    std::cout << "CornerNum: " << CornerCoordiantes.size() << std::endl;
    Vector3UC color;
    color.x = 0; color.y = 0; color.z = 0;
    DrawPointToImageRGB(image, image, width, height, color, CornerCoordiantes);

    stbi_write_jpg(outImgPath.c_str(), width, height, STBI_rgb, image, 100);
    stbi_image_free(image);
    FREE_Memory(grayImageBuffer);

    return 0;
}
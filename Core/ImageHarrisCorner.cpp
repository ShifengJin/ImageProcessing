#include <memory.h>
#include <malloc.h>
#include <math.h>
#include <iostream>
#include "ImageBlur.h"
#include "Common.h"
#include "ImageHarrisCorner.h"
#include "ImageTransformation.h"

static void M(float* gray, int width, int height, float* oIx2, float* oIy2, float* IxIy){

    memset(oIx2, 0, width * height * sizeof(float));
    memset(oIy2, 0, width * height * sizeof(float));
    memset(IxIy, 0, width * height * sizeof(float));

    float* inGray = gray;
    float* outIx = oIx2;
    // Ix
    float ixKernel[9] = {
        -1.f, 0.f, 1.f,
        -1.f, 0.f, 1.f,
        -1.f, 0.f, 1.f
    };
    Utily::ImageConvolution(inGray, width, height, 1, outIx, ixKernel, 3);
    // Iy
    float* outIy = oIy2;
    float iyKernel[9] = {
        -1.f, -1.f, -1.f,
         0.f,  0.f,  0.f,
         1.f,  1.f,  1.f
    };
    Utily::ImageConvolution(inGray, width, height, 1, outIy, iyKernel, 3);

    for(int i = 0; i < height; ++ i){
        for(int j = 0; j < width; ++ j){
            *(IxIy) = *(oIx2) * *(oIy2);
            *(oIx2) = *(oIx2) * *(oIx2);
            *(oIy2) = *(oIy2) * *(oIy2);
            IxIy += 1;
            oIx2 += 1;
            oIy2 += 1;
        }
    }
}

static bool checkIsLocalMax(float* mr, int x, int y, int width, int localSize, float value){
    int offset = localSize / 2;
    for(int m = -offset; m <= offset; ++ m){
        for(int n = -offset; n <= offset; ++ n){
            if(m == 0 && n == 0) continue;
            if(value < mr[(y + m) * width + x + n]){
                return false;
            }
        }
    }
    return true;
}

void HarrisCorner(float* gray, int width, int height, std::vector<Coordinate_i>& oCornerCoordiantes, float k, int ksize, float* GaussFilter, float* Ix2, float* Iy2, float* IxIy, float* Gauss_Ix2, float* Gauss_Iy2, float* Gauss_IxIy, float* MR, int localSize){

    M(gray, width, height, Ix2, Iy2, IxIy);

    Utily::ImageConvolution(Ix2, width, height, 1, Gauss_Ix2, GaussFilter, ksize);
    Utily::ImageConvolution(Iy2, width, height, 1, Gauss_Iy2, GaussFilter, ksize);
    Utily::ImageConvolution(IxIy, width, height, 1, Gauss_IxIy, GaussFilter, ksize);
    int size = width * height;

    float absMaxValue = 0.f;
    
    for(int i = 0; i < size; ++ i){
        // Ix2 * Iy2 - IxIy * IxIy - k * (Ix2 + Iy2)^2
        float r = (Gauss_Ix2[i] * Gauss_Iy2[i] - Gauss_IxIy[i] * Gauss_IxIy[i]) - k * (Gauss_Ix2[i] + Gauss_Iy2[i]) * (Gauss_Ix2[i] + Gauss_Iy2[i]);
        MR[i] = fabsf(r);
        if(MR[i] > absMaxValue){
            absMaxValue = MR[i];
        }
    }

    // Local maximum and threshold suppression
    float threshold = 0.01f * absMaxValue;
    //float threshold = 0.f;
    std::cout << "absMaxValue: " << absMaxValue << "    threshold: " << threshold << std::endl;

    int startY = localSize / 2;
    int endY = height - localSize / 2;
    int startX = localSize / 2;
    int endX = width - localSize / 2;
    for(int i = startY; i < endY; ++ i){
        for(int j = startX; j < endX; ++ j){
            float v = MR[i * width + j];
            if(v > threshold && checkIsLocalMax(MR, j, i, width, localSize, v)){                
               oCornerCoordiantes.push_back({j, i});
            }
        }
    }
}
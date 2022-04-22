#include <memory.h>
#include <malloc.h>
#include "ImageInterpolation.h"

void ImageInterpolation_Init(int width, int height, int channels, float scale, int* oWidth, int* oHeight, unsigned char** oBuffer){
    *oWidth = (int)(width * scale + 0.5f);
    *oHeight = (int)(height * scale + 0.5f);
    *oBuffer = (unsigned char*)calloc(*oWidth * *oHeight * channels, sizeof(unsigned char));
}

void ImageInterpolation_Init(int width, int height, int channels, float scale, int* oWidth, int* oHeight, float** oBuffer){
    *oWidth = (int)(width * scale + 0.5f);
    *oHeight = (int)(height * scale + 0.5f);
    *oBuffer = (float*)calloc(*oWidth * *oHeight * channels, sizeof(float));
}

void ImageInterpolation_Nearest(
    unsigned char* src, int width, int height, int channels,
    unsigned char* dst, int oWidth, int oHeight){
    
    float scalex = width * 1.f / oWidth;
    float scaley = height * 1.f / oHeight;

    for(int i = 0; i < oHeight; ++ i){
        int y = (int)(i * scaley + 0.5f);
        for(int j = 0; j < oWidth; ++ j){
            int x = (int)(j * scalex + 0.5f);
            for(int c = 0; c < channels; ++ c){
                dst[(i * oWidth + j) * channels + c] = src[(y * width + x) * channels + c];
            }
        }
    }
}

void ImageInterpolation_Nearest(
    float* src, int width, int height, int channels,
    float* dst, int oWidth, int oHeight){
    
    float scalex = width * 1.f / oWidth;
    float scaley = height * 1.f / oHeight;

    for(int i = 0; i < oHeight; ++ i){
        int y = (int)(i * scaley + 0.5f);
        for(int j = 0; j < oWidth; ++ j){
            int x = (int)(j * scalex + 0.5f);
            for(int c = 0; c < channels; ++ c){
                dst[(i * oWidth + j) * channels + c] = src[(y * width + x) * channels + c];
            }
        }
    }
}

void ImageInterpolation_Unit(unsigned char** buffer){
    if(buffer){
        if(*buffer){
            free(*buffer);
            *buffer = NULL;
        }
    }
}

void ImageInterpolation_Unit(float** buffer){
    if(buffer){
        if(*buffer){
            free(*buffer);
            *buffer = NULL;
        }
    }
}
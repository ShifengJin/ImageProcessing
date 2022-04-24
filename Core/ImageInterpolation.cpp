#include <memory.h>
#include <malloc.h>
#include <math.h>
#include <vector>
#include <iostream>
#include "Common.h"
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

void ImageInterpolation_Linear(
    unsigned char* src, int width, int height, int channels,
    unsigned char* dst, int oWidth, int oHeight){
    
    float scalex = 1.f * width / oWidth;
    float scaley = 1.f * height / oHeight;

    for(int i = 0; i < oHeight; ++ i){
        float y = i * scaley;
        int yUp = (int)y;
        int yDown = yUp + 1;
        float upWeight = yDown - y;
        float downWeight = 1.f - upWeight;
        for(int j = 0; j < oWidth; ++ j){
            float x = j * scalex;
            int xLeft = (int)x;
            int xRight = xLeft + 1;
            float leftWeight = xRight - x;
            float rightWeight = 1.f - leftWeight;
            
            for(int c = 0; c < channels; ++ c){
                float v = (src[(yUp * width + xLeft) * channels + c] * leftWeight + 
                           src[(yUp * width + xRight) * channels + c] * rightWeight) * upWeight + 
                          (src[(yDown * width + xLeft) * channels + c] * leftWeight + 
                           src[(yDown * width + xRight) * channels + c] * rightWeight) * downWeight;
                
                dst[(i * oWidth + j) * channels + c] = v > 255.f ? (unsigned char)255 : (v < 0.f ? (unsigned char)0 : (unsigned char)v);
            }
        }
    }
}

void ImageInterpolation_Linear(
    float* src, int width, int height, int channels,
    float* dst, int oWidth, int oHeight){

    float scalex = 1.f * width / oWidth;
    float scaley = 1.f * height / oHeight;

    for(int i = 0; i < oHeight; ++ i){
        float y = i * scaley;
        int yUp = (int)y;
        int yDown = yUp + 1;
        float upWeight = yDown - y;
        float downWeight = 1.f - upWeight;
        for(int j = 0; j < oWidth; ++ j){
            float x = j * scalex;
            int xLeft = (int)x;
            int xRight = xLeft + 1;
            float leftWeight = xRight - x;
            float rightWeight = 1.f - leftWeight;
            
            for(int c = 0; c < channels; ++ c){
                float v = (src[(yUp * width + xLeft) * channels + c] * leftWeight + 
                           src[(yUp * width + xRight) * channels + c] * rightWeight) * upWeight + 
                          (src[(yDown * width + xLeft) * channels + c] * leftWeight + 
                           src[(yDown * width + xRight) * channels + c] * rightWeight) * downWeight;
                
                dst[(i * oWidth + j) * channels + c] = v;
            }
        }
    }
}

float ImageInterpolation_Cubi_ComputeWeight(float x){
    float absX = fabsf(x);
    if(absX >= 0 && absX < 1.f){
        return (1.f - 2.f * absX * absX + absX * absX * absX);
    }else if(absX >= 1.f && absX < 2.f){
        return (4.f - 8.f * absX + 5.f * absX * absX - absX * absX * absX);
    }else{
        return 0.f;
    }
}

void ImageInterpolation_Cubic(
    unsigned char* src, int width, int height, int channels,
    unsigned char* dst, int oWidth, int oHeight){
    
    float scalex = 1.f * width / oWidth;
    float scaley = 1.f * height / oHeight;
    
    for(int i = 0; i < oHeight; ++i){
        float y = i * scaley;
        int yUp = (int)y;
        int yUpUp = yUp - 1;
        int yDown = yUp + 1;
        int yDownDown = yDown + 1;
        
        float yUpUpWeight, yUpWeight, yDownWeight, yDownDownWeight;
        if(yUpUp < 0){
            yUpUpWeight = 0.f;
            yUpUp = 0;
        }else{
            yUpUpWeight = ImageInterpolation_Cubi_ComputeWeight(yUpUp - y);
        }
        
        yUpWeight = ImageInterpolation_Cubi_ComputeWeight(yUp - y);
        yDownWeight = ImageInterpolation_Cubi_ComputeWeight(yDown - y);

        if(yDownDown >= height){
            yDownDownWeight = 0.f;
            yDownDown = height - 1;
        }else{
            yDownDownWeight = ImageInterpolation_Cubi_ComputeWeight(yDownDown - y);
        }


        for(int j = 0; j < oWidth; ++ j){
            float x = j * scalex;
            int xLeft = (int)x;
            int xLeftLeft = xLeft - 1;
            int xRight = xLeft + 1;
            int xRightRight = xRight + 1;
            float xLeftLeftWeight, xLeftWeight, xRightWeight, xRightRightWeight;
            if(xLeftLeft < 0){
                xLeftLeftWeight = 0.f;
                xLeftLeft = 0;
            }else{
                xLeftLeftWeight = ImageInterpolation_Cubi_ComputeWeight(xLeftLeft - x);
            }
            xLeftWeight = ImageInterpolation_Cubi_ComputeWeight(xLeft - x);
            xRightWeight = ImageInterpolation_Cubi_ComputeWeight(xRight - x);
            if(xRightRight >= width){
                xRightRightWeight = 0.f;
                xRightRight = width - 1;
            }else{
                xRightRightWeight = ImageInterpolation_Cubi_ComputeWeight(xRightRight - x);
            }
            
            float weight[16] = {
                yUpUpWeight    *xLeftLeftWeight, yUpUpWeight    *xLeftWeight, yUpUpWeight    *xRightWeight, yUpUpWeight    *xRightRightWeight,
                yUpWeight      *xLeftLeftWeight, yUpWeight      *xLeftWeight, yUpWeight      *xRightWeight, yUpWeight      *xRightRightWeight,
                yDownWeight    *xLeftLeftWeight, yDownWeight    *xLeftWeight, yDownWeight    *xRightWeight, yDownWeight    *xRightRightWeight,
                yDownDownWeight*xLeftLeftWeight, yDownDownWeight*xLeftWeight, yDownDownWeight*xRightWeight, yDownDownWeight*xRightRightWeight,
            };            

            for(int c = 0; c < channels; ++ c){
                float v = src[(yUpUp     * width + xLeftLeft) * channels + c] * weight[0 ] + src[(yUpUp     * width + xLeft) * channels + c] * weight[1 ] + src[(yUpUp     * width + xRight) * channels + c] * weight[2 ] + src[(yUpUp     * width + xRightRight) * channels + c] * weight[3 ] + 
                          src[(yUp       * width + xLeftLeft) * channels + c] * weight[4 ] + src[(yUp       * width + xLeft) * channels + c] * weight[5 ] + src[(yUp       * width + xRight) * channels + c] * weight[6 ] + src[(yUp       * width + xRightRight) * channels + c] * weight[7 ] + 
                          src[(yDown     * width + xLeftLeft) * channels + c] * weight[8 ] + src[(yDown     * width + xLeft) * channels + c] * weight[9 ] + src[(yDown     * width + xRight) * channels + c] * weight[10] + src[(yDown     * width + xRightRight) * channels + c] * weight[11] + 
                          src[(yDownDown * width + xLeftLeft) * channels + c] * weight[12] + src[(yDownDown * width + xLeft) * channels + c] * weight[13] + src[(yDownDown * width + xRight) * channels + c] * weight[14] + src[(yDownDown * width + xRightRight) * channels + c] * weight[15];
                dst[(i * oWidth + j) * channels + c] = v > 255.f ? (unsigned char)255 : ((v < 0.f ? (unsigned char)0 : (unsigned char)v));
            }            
        }
    }
}

void ImageInterpolation_Cubic(
    float* src, int width, int height, int channels,
    float* dst, int oWidth, int oHeight){

    float scalex = 1.f * width / oWidth;
    float scaley = 1.f * height / oHeight;
    
    for(int i = 0; i < oHeight; ++i){
        float y = i * scaley;
        int yUp = (int)y;
        int yUpUp = yUp - 1;
        int yDown = yUp + 1;
        int yDownDown = yDown + 1;
        
        float yUpUpWeight, yUpWeight, yDownWeight, yDownDownWeight;
        if(yUpUp < 0){
            yUpUpWeight = 0.f;
            yUpUp = 0;
        }else{
            yUpUpWeight = ImageInterpolation_Cubi_ComputeWeight(yUpUp - y);
        }
        
        yUpWeight = ImageInterpolation_Cubi_ComputeWeight(yUp - y);
        yDownWeight = ImageInterpolation_Cubi_ComputeWeight(yDown - y);

        if(yDownDown >= height){
            yDownDownWeight = 0.f;
            yDownDown = height - 1;
        }else{
            yDownDownWeight = ImageInterpolation_Cubi_ComputeWeight(yDownDown - y);
        }


        for(int j = 0; j < oWidth; ++ j){
            float x = j * scalex;
            int xLeft = (int)x;
            int xLeftLeft = xLeft - 1;
            int xRight = xLeft + 1;
            int xRightRight = xRight + 1;
            float xLeftLeftWeight, xLeftWeight, xRightWeight, xRightRightWeight;
            if(xLeftLeft < 0){
                xLeftLeftWeight = 0.f;
                xLeftLeft = 0;
            }else{
                xLeftLeftWeight = ImageInterpolation_Cubi_ComputeWeight(xLeftLeft - x);
            }
            xLeftWeight = ImageInterpolation_Cubi_ComputeWeight(xLeft - x);
            xRightWeight = ImageInterpolation_Cubi_ComputeWeight(xRight - x);
            if(xRightRight >= width){
                xRightRightWeight = 0.f;
                xRightRight = width - 1;
            }else{
                xRightRightWeight = ImageInterpolation_Cubi_ComputeWeight(xRightRight - x);
            }
            
            float weight[16] = {
                yUpUpWeight    *xLeftLeftWeight, yUpUpWeight    *xLeftWeight, yUpUpWeight    *xRightWeight, yUpUpWeight    *xRightRightWeight,
                yUpWeight      *xLeftLeftWeight, yUpWeight      *xLeftWeight, yUpWeight      *xRightWeight, yUpWeight      *xRightRightWeight,
                yDownWeight    *xLeftLeftWeight, yDownWeight    *xLeftWeight, yDownWeight    *xRightWeight, yDownWeight    *xRightRightWeight,
                yDownDownWeight*xLeftLeftWeight, yDownDownWeight*xLeftWeight, yDownDownWeight*xRightWeight, yDownDownWeight*xRightRightWeight,
            };            

            for(int c = 0; c < channels; ++ c){
                float v = src[(yUpUp     * width + xLeftLeft) * channels + c] * weight[0 ] + src[(yUpUp     * width + xLeft) * channels + c] * weight[1 ] + src[(yUpUp     * width + xRight) * channels + c] * weight[2 ] + src[(yUpUp     * width + xRightRight) * channels + c] * weight[3 ] + 
                          src[(yUp       * width + xLeftLeft) * channels + c] * weight[4 ] + src[(yUp       * width + xLeft) * channels + c] * weight[5 ] + src[(yUp       * width + xRight) * channels + c] * weight[6 ] + src[(yUp       * width + xRightRight) * channels + c] * weight[7 ] + 
                          src[(yDown     * width + xLeftLeft) * channels + c] * weight[8 ] + src[(yDown     * width + xLeft) * channels + c] * weight[9 ] + src[(yDown     * width + xRight) * channels + c] * weight[10] + src[(yDown     * width + xRightRight) * channels + c] * weight[11] + 
                          src[(yDownDown * width + xLeftLeft) * channels + c] * weight[12] + src[(yDownDown * width + xLeft) * channels + c] * weight[13] + src[(yDownDown * width + xRight) * channels + c] * weight[14] + src[(yDownDown * width + xRightRight) * channels + c] * weight[15];
                dst[(i * oWidth + j) * channels + c] = v;
            }            
        }
    }
}

float ImageInterpolation_Lanczos_ComputeWeight(float x, int A){
    if(fabsf(x) < 1e-4f){
        return 1.f;
    }

    if(x > -A && x < A){
        return (A * sinf(PI * x) * sinf(PI * x / A) / (PI * PI * x * x));
    }else{
        return 0.f;
    }
}


typedef struct _LanczosWeightParam_{
    std::vector<int> index;
    std::vector<float> weight;
}LanczosWeightParam;

void ImageInterpolation_Lanczos(
    unsigned char* src, int width, int height, int channels,
    unsigned char* dst, int oWidth, int oHeight, int A){
    
    float scaleX = width * 1.0f / oWidth;
    float scaleY = height * 1.0f / oHeight;

    LanczosWeightParam yLanczosWeightParam;
    LanczosWeightParam xLanczosWeightParam;
    yLanczosWeightParam.index.resize(A * 2);
    yLanczosWeightParam.weight.resize(A * 2);
    xLanczosWeightParam.index.resize(A * 2);
    xLanczosWeightParam.weight.resize(A * 2);    

    for(int i = 0; i < oHeight; ++ i){
        float y = i * scaleY;
        for(int yIter = 0; yIter < A * 2; ++ yIter){
            yLanczosWeightParam.index[yIter] = (int)y - (A - yIter) + 1;
            if(yLanczosWeightParam.index[yIter] < 0){
                yLanczosWeightParam.index[yIter] = 0;
                yLanczosWeightParam.weight[yIter] = 0.f;
            }else{
                yLanczosWeightParam.weight[yIter] = ImageInterpolation_Lanczos_ComputeWeight(yLanczosWeightParam.index[yIter] - y, A);
            }
            //std::cout << yLanczosWeightParam.weight[yIter] << ", ";
        }
        //std::cout << std::endl;
        //std::cout << "========================================================" << std::endl;
        for(int j = 0; j < oWidth; ++ j){
            float x = j * scaleX;
            for(int xIter = 0; xIter < A * 2; ++ xIter){
                xLanczosWeightParam.index[xIter] = (int)x - (A - xIter) + 1;
                if(xLanczosWeightParam.index[xIter] < 0){
                    xLanczosWeightParam.index[xIter] = 0;
                    xLanczosWeightParam.weight[xIter] = 0.f;
                }else{
                    xLanczosWeightParam.weight[xIter] = ImageInterpolation_Lanczos_ComputeWeight(xLanczosWeightParam.index[xIter] - x, A);
                }
            }
            
            std::vector<float> channelsSum(channels, 0);

            for(int yIter = 0; yIter< A * 2; ++ yIter){
                float yOffset = yLanczosWeightParam.index[yIter];
                float yWeight = yLanczosWeightParam.weight[yIter];
                for(int xIter = 0; xIter < A * 2; ++ xIter){
                    float xOffset = xLanczosWeightParam.index[xIter];
                    float xWeight = xLanczosWeightParam.weight[xIter];
                    float weight = yWeight * xWeight;
                    int index = (yOffset * width + xOffset) * channels;
                    for(int c = 0; c < channels; ++ c){
                        channelsSum[c] += (src[index + c] * weight);
                    }
                }
            }
            for(int c = 0; c < channels; ++ c){
                dst[(i * oWidth + j) * channels + c] = CLAMP(channelsSum[c], (unsigned char)0, (unsigned char)255);
            }

        }
    }

}

void ImageInterpolation_Lanczos(
    float* src, int width, int height, int channels,
    float* dst, int oWidth, int oHeight, int A){
    float scaleX = width * 1.0f / oWidth;
    float scaleY = height * 1.0f / oHeight;

    LanczosWeightParam yLanczosWeightParam;
    LanczosWeightParam xLanczosWeightParam;
    yLanczosWeightParam.index.resize(A * 2);
    yLanczosWeightParam.weight.resize(A * 2);
    xLanczosWeightParam.index.resize(A * 2);
    xLanczosWeightParam.weight.resize(A * 2);    

    for(int i = 0; i < oHeight; ++ i){
        float y = i * scaleY;
        for(int yIter = 0; yIter < A * 2; ++ yIter){
            yLanczosWeightParam.index[yIter] = (int)y - (A - yIter) + 1;
            if(yLanczosWeightParam.index[yIter] < 0){
                yLanczosWeightParam.index[yIter] = 0;
                yLanczosWeightParam.weight[yIter] = 0.f;
            }else{
                yLanczosWeightParam.weight[yIter] = ImageInterpolation_Lanczos_ComputeWeight(yLanczosWeightParam.index[yIter] - y, A);
            }
        }

        for(int j = 0; j < oWidth; ++ j){
            float x = j * scaleX;
            for(int xIter = 0; xIter < A * 2; ++ xIter){
                xLanczosWeightParam.index[xIter] = (int)x - (A - xIter) + 1;
                if(xLanczosWeightParam.index[xIter] < 0){
                    xLanczosWeightParam.index[xIter] = 0;
                    xLanczosWeightParam.weight[xIter] = 0.f;
                }else{
                    xLanczosWeightParam.weight[xIter] = ImageInterpolation_Lanczos_ComputeWeight(xLanczosWeightParam.index[xIter] - x, A);
                }
            }
            
            std::vector<float> channelsSum(channels, 0);

            for(int yIter = 0; yIter< A * 2; ++ yIter){
                float yOffset = yLanczosWeightParam.index[yIter];
                float yWeight = yLanczosWeightParam.weight[yIter];
                for(int xIter = 0; xIter < A * 2; ++ xIter){
                    float xOffset = xLanczosWeightParam.index[xIter];
                    float xWeight = xLanczosWeightParam.weight[xIter];
                    float weight = yWeight * xWeight;
                    int index = (yOffset * width + xOffset) * channels;
                    for(int c = 0; c < channels; ++ c){
                        channelsSum[c] += src[index + c] * weight;
                    }
                }
            }
            for(int c = 0; c < channels; ++ c){
                dst[(i * oWidth + j) * channels + c] = channelsSum[c];
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
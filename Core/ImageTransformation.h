#ifndef _IMAGETRANSFORMATION_H_
#define _IMAGETRANSFORMATION_H_

#include <vector>
#include "Common.h"

void RGB2Gray(unsigned char* rgb, unsigned char* gray, int width, int height);
void RGB2Gray(float* rgb, float* gray, int width, int height);

template<typename T, typename O>
void FindMinMax(T* in, int size, O* minV, O* maxV);

template<typename I, typename O>
void Normalization(I* in, O* out, int size, float minV, float maxV);


void DrawPointToImageRGB(unsigned char* rgb, unsigned char* oRgb, int width, int height, Color_UChar color, std::vector<Coordinate_i> coordinates);
void DrawPointToImageGray(unsigned char* gray, unsigned char* oGray, int width, int height, unsigned char color, std::vector<Coordinate_i> coordinates);

template<typename T, typename O>
void FindMinMax(T* in, int size, O* minV, O* maxV) {
    *minV = (O)999999;
    *maxV = -(*minV);
    for (int i = 0; i < size; ++i) {
        O v = (O)(in[i]);
        if (v > *maxV) {
            *maxV = v;
        }
        if (v < *minV) {
            *minV = v;
        }
    }
}

template<typename I, typename O>
void Normalization(I* in, O* out, int size, float minV, float maxV) {
    I minValue = (I)99999, maxValue = (I)-99999;
    FindMinMax<I, I>(in, size, &minValue, &maxValue);

    // 归一化
    float normalizeOneInv = 1.f / (maxValue - minValue) * (maxV - minV);
    for (int i = 0; i < size; ++i) {
        out[i] = (O)((float)(in[i] - minValue) * normalizeOneInv + minV);
    }
}

#endif
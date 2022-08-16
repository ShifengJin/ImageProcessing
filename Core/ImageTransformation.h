#ifndef _IMAGETRANSFORMATION_H_
#define _IMAGETRANSFORMATION_H_

#include <vector>
#include "Common.h"

void DrawPointToImageRGB(unsigned char* rgb, unsigned char* oRgb, int width, int height, Color_UChar color, std::vector<Coordinate_i> coordinates);
void DrawPointToImageGray(unsigned char* gray, unsigned char* oGray, int width, int height, unsigned char color, std::vector<Coordinate_i> coordinates);

class Utily{
public:

    static void RGB2Gray(unsigned char* rgb, unsigned char* gray, int width, int height);

    static void RGB2Gray(unsigned char* rgb, float* gray, int width, int height);

    // normalize rgb2gray
    static void RGB2Gray(float* rgb, float* gray, int width, int height);

    template<typename T>
    static void FindMinMax(T* in, int size, T& oMin, T& oMax);

    template<typename I, typename O>
    static void Normalize(I* in, O* out, int size, O minValue, O maxValue);
};

template<typename T>
inline void Utily::FindMinMax(T* in, int size, T& oMin, T& oMax){
    oMin = (T)999999999;
    oMax = (T)-9999999999;

    for(int i = 0; i < size; ++ i){
        if(in[i] > oMax){
            oMax = in[i];
        }
        if(in[i] < oMin){
            oMin = in[i];
        }
    }
}

template<typename I, typename O>
inline void Utily::Normalize(I* in, O* out, int size, O minValue, O maxValue){
    I minV, maxV;
    Utily::FindMinMax<I>(in, size, minV, maxV);
    double inv = 1. / (maxV - minV);
    double diff = maxValue - minValue;
    for(int i = 0; i < size; ++ i){
        out[i] = (O)(((double)in[i] - minV) * inv * diff + minValue);
    }
}

#endif
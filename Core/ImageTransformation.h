#ifndef _IMAGETRANSFORMATION_H_
#define _IMAGETRANSFORMATION_H_

#include "stb_image.h"
#include "stb_image_write.h"

#include <vector>
#include <string>
#include "Common.h"

void DrawPointToImageRGB(unsigned char* rgb, unsigned char* oRgb, int width, int height, Vector3UC& color, std::vector<Vector2i>& coordinates);
void DrawPointToImageGray(unsigned char* gray, unsigned char* oGray, int width, int height, unsigned char color, std::vector<Vector2i> coordinates);

class Utily{
public:

    static void GaussFilter(int ksize, float sigma, float* ofilter);

    static void MotionFilter(int ksize, float angle, float* ofilter);

    static void BilaternalColorFilter(float* ofilter, float sigma);

    static void ImageConvolution(unsigned char* src, int width, int height, int channels, unsigned char* dst, float* filter, int ksize);

    static void ImageConvolution(float* src, int width, int height, int channels, float* dst, float* filter, int ksize);
    static void ImageConvolution1(float* src, int width, int height, int channels, float* dst, float* filter, int ksize);

    static void TwoImageDiff(float* img0, float* img1, int width, int height, int channels, float* out);

    static void RGB2Gray(unsigned char* rgb, unsigned char* gray, int width, int height);

    static void RGB2Gray(unsigned char* rgb, float* gray, int width, int height);

    // normalize rgb2gray
    static void RGB2Gray(float* rgb, float* gray, int width, int height);

    template<typename T>
    static void FindMinMax(T* in, int size, T& oMin, T& oMax);

    template<typename I, typename O>
    static void Normalize(I* in, O* out, int size, O minValue, O maxValue);

    template<typename T>
    static void SaveImage(std::string path, int width, int height, int channels, T* img);

private:
    static void ConvUInt8(unsigned char* in, unsigned char* out, int width, int channels, float* filter, int ksize);
    static void ConvFloat(float* in, float* out, int width, int channels, float* filter, int ksize);
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

template<typename T>
inline void Utily::SaveImage(std::string path, int width, int height, int channels, T* img){
    unsigned char* saveBuffer = (unsigned char*)calloc(width * height * channels, sizeof(unsigned char));
    Utily::Normalize<T, unsigned char>(img, saveBuffer, width * height * channels, (unsigned char)0, (unsigned char)255);
    if(channels == 1){
        stbi_write_jpg(path.c_str(), width, height, STBI_grey, saveBuffer, 100);
    }else if(channels == 3){
        stbi_write_jpg(path.c_str(), width, height, STBI_rgb, saveBuffer, 100);
    }
    FREE_Memory(saveBuffer);
}

#endif
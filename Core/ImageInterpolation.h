#ifndef _IMAGEINTERPOLATION_H_
#define _IMAGEINTERPOLATION_H_

typedef enum _INTERPOLATION_TYPE_{
    NEAREST = 0,
    LINEAR = 1,
}INTERPOLATIONTYPE;


void ImageInterpolation_Init(int width, int height, int channels, float scale, int* oWidth, int* oHeight, unsigned char** oBuffer);
void ImageInterpolation_Init(int width, int height, int channels, float scale, int* oWidth, int* oHeight, float** oBuffer);

void ImageInterpolation_Nearest(
    unsigned char* src, int width, int height, int channels,
    unsigned char* dst, int oWidth, int oHeight);

void ImageInterpolation_Nearest(
    float* src, int width, int height, int channels,
    float* dst, int oWidth, int oHeight);

void ImageInterpolation_Linear(
    unsigned char* src, int width, int height, int channels,
    unsigned char* dst, int oWidth, int oHeight);

void ImageInterpolation_Linear(
    float* src, int width, int height, int channels,
    float* dst, int oWidth, int oHeight);


void ImageInterpolation_Unit(unsigned char** buffer);
void ImageInterpolation_Unit(float** buffer);

#endif
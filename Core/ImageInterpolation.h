#ifndef _IMAGEINTERPOLATION_H_
#define _IMAGEINTERPOLATION_H_

typedef enum _INTERPOLATION_TYPE_{
    NEAREST = 0,
    LINEAR = 1,
    CUBIC = 2,
    LANCZOS = 3,
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

//         / //
//        | 1.f - 2.f * x * x + abs(x*x*x)                                        , 0.f <= abs(x) < 1.f
// s(x) = | 4.f - 8.f * abs(x) + 5.f * x * x - abs(x*x*x)                         , 1.f <= abs(x) < 2.f
//        | 0                                                                     , abs(x) >= 2.f
//         \ //
float ImageInterpolation_Cubi_ComputeWeight(float x);
void ImageInterpolation_Cubic(
    unsigned char* src, int width, int height, int channels,
    unsigned char* dst, int oWidth, int oHeight);
void ImageInterpolation_Cubic(
    float* src, int width, int height, int channels,
    float* dst, int oWidth, int oHeight);

//         /
//        | sinc(x)sinc(x/a)                 , -a < x < a
// L(x) = |
//        | 0                                , otherwise
//         \ 
// Equivalently
//        /
//        | 1                                       , x = 0
// L(x) = | a*sin(pi*x)*sin(pi*x/a) / (pi*pi*x*x)   , 0 < |x| < a
//        | 0                                       , otherwise
//        \ 
// A recommend 2 or 3
float ImageInterpolation_Lanczos_ComputeWeight(float x, int A);
void ImageInterpolation_Lanczos(
    unsigned char* src, int width, int height, int channels,
    unsigned char* dst, int oWidth, int oHeight, int A);
void ImageInterpolation_Lanczos(
    float* src, int width, int height, int channels,
    float* dst, int oWidth, int oHeight, int A);

void ImageInterpolation_Unit(unsigned char** buffer);
void ImageInterpolation_Unit(float** buffer);

#endif
#ifndef _IMAGEBLUR_H_
#define _IMAGEBLUR_H_

typedef enum _BLURTYPE_{
    GAUSS = 0,
    MOTION = 1,
    MEDIAN = 2,
    AVG = 3,
    GLASS = 4
}BLURTYPE;

void GaussFilter(int ksize, float sigma, float* ofilter);
void MotionFilter(int ksize, float angle, float* ofilter);

void ImageConvolution(
    unsigned char* src, int width, int height, int channels,
    unsigned char* dst, float* filter, int ksize);

void ImageConvolution(
    float* src, int width, int height, int channels,
    float* dst, float* filter, int ksize);

void ImageMedian(unsigned char* src, int width, int height, int channels, unsigned char* dst, int ksize);
void ImageMedian(float* src, int width, int height, int channels, float* dst, int ksize);

void ImageAvg(unsigned char* src, int width, int height, int channels, unsigned char* dst, int ksize);
void ImageAvg(float* src, int width, int height, int channels, float* dst, int ksize);

void ImageGlass(unsigned char* src, int width, int height, int channels, unsigned char* dst, int ksize);
void ImageGlass(float* src, int width, int height, int channels, float* dst, int ksize);

#endif
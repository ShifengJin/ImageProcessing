#ifndef _IMAGEROTATE_H_
#define _IMAGEROTATE_H_

void ImageRotate(
    unsigned char* src, int width, int height, int channels,
    unsigned char* dst, float angle);

void ImageRotate(
    float* src, int width, int height, int channels,
    float* dst, float angle);

#endif
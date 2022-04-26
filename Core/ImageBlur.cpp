#include <math.h>
#include <vector>
#include <memory.h>
#include "Common.h"
#include "ImageBlur.h"

void GaussFilter(int ksize, float sigma, float* ofilter){
    if (ksize == 1) {
        ofilter[0] = 1.f;
        return;
    }
    int bufferSize = ksize * ksize;
    float sigmasigma2 = 2.0f * sigma * sigma;
    float sigmasigma2Inv = 1.f / sigmasigma2;
    float sigmasigma2PIInv = sigmasigma2Inv / PI;
    int radius = ksize / 2;
    float sum = 0.f;
    for (int i = -radius; i <= radius; ++i) {
        for (int j = -radius; j <= radius; ++j) {
            ofilter[(i + radius) * ksize + (j + radius)] = sigmasigma2PIInv * expf(-(i * i + j * j) * sigmasigma2Inv);
        }
    }

    for (int i = 0; i < bufferSize; ++i) {
        sum += ofilter[i];
    }
    sum = 1.f / sum;
    for (int i = 0; i < bufferSize; ++i) {
        ofilter[i] = ofilter[i] * sum;
    }
}

void MotionFilter(int ksize, float angle, float* ofilter){
    float tangle = angle * ANGLE2RAD;

    float rotate[4] = {
         cosf(tangle), sinf(tangle),
        -sinf(tangle), cosf(tangle)
    };

    float* zeroAngleFilter = (float*)calloc(ksize * ksize, sizeof(float));

    memset(ofilter, 0, ksize * ksize * sizeof(float));

    int halfsize = ksize / 2;
    int yoffset = halfsize * ksize;
    float filterV = 1.f / ksize;
    for (int i = 0; i < ksize; ++i) {
        zeroAngleFilter[yoffset + i] = filterV;
    }
    
    float* tmpOut = ofilter;
    float* tmpIn = zeroAngleFilter;
    float sum = 0.f;
    int index = 0;
    
    for (int i = -halfsize; i <= halfsize; ++i) {

        for (int j = -halfsize; j <= halfsize; ++j) {

            float x = rotate[0] * j + rotate[1] * i + halfsize;
            float y = rotate[2] * j + rotate[3] * i + halfsize;

            int xsmall = (int)x;
            int xbig = xsmall + 1;
            int ysmall = (int)y;
            int ybig = ysmall + 1;

            xsmall = CLAMP(xsmall, 0, (ksize - 1));
            xbig = CLAMP(xbig, 0, (ksize - 1));
            ysmall = CLAMP(ysmall, 0, (ksize - 1));
            ybig = CLAMP(ybig, 0, (ksize - 1));

            float xWeight1 = xbig - x;
            float xWeight2 = 1.f - xWeight1;
            float yWeight1 = ybig - y;
            float yWeight2 = 1.f - yWeight1;

            float y1 = tmpIn[ysmall * ksize + xsmall] * xWeight1 + tmpIn[ysmall * ksize + xbig] * xWeight2;
            float y2 = tmpIn[ybig * ksize + xsmall] * xWeight1 + tmpIn[ybig * ksize + xbig] * xWeight2;
            float value = y1 * yWeight1 + y2 * yWeight2;

            *tmpOut = value;
            tmpOut++;
            index++;
            sum += value;
            
        }
    }
    
    sum = 1.f / sum;
    for (int i = 0; i < ksize * ksize; ++i) {
        ofilter[i] = ofilter[i] * sum;
    }

    if (zeroAngleFilter) {
        free(zeroAngleFilter);
        zeroAngleFilter = NULL;
    }
}

void ConvUInt8(unsigned char* in, unsigned char* out, int width, int channels, float* filter, int ksize) {

    int lineSize = width * channels;
    unsigned char* inTemp = in;
    std::vector<float> sum(channels, 0);
    for (int i = 0; i < ksize; ++i) {
        for (int j = 0; j < ksize; ++j) {
            int xoffset = j * channels;
            for(int c = 0; c < channels; ++ c){
                sum[c] += (*filter) * inTemp[xoffset + c];
            }
            filter++;
        }
        inTemp = inTemp + lineSize;
    }
    for(int c = 0; c < channels; ++ c){
        out[c] = CLAMP(sum[c], (unsigned char)0, (unsigned char)255);
    }
}

void ConvFloat(float* in, float* out, int width, int channels, float* filter, int ksize) {

    int lineSize = width * channels;
    float* inTemp = in;
    
    for (int i = 0; i < ksize; ++i) {
        for (int j = 0; j < ksize; ++j) {
            int xoffset = j * channels;
            for(int c = 0; c < channels; ++ c){
                out[c] += (*filter) * inTemp[xoffset + c];
            }
            filter++;
        }
        inTemp = inTemp + lineSize;
    }
}

void ImageConvolution(
    unsigned char* src, int width, int height, int channels,
    unsigned char* dst, float* filter, int ksize){

    size_t size = (size_t)width * (size_t)height * sizeof(unsigned char) * channels;

    int startX = ksize / 2;
    int endX = width - ksize / 2;

    int startY = ksize / 2;
    int endY = height - ksize / 2;

    unsigned char* tempOut = dst + (startY * width + startX) * 3;

    memset(dst, 0, size);
    
    for (int i = 0; i <= height - ksize; ++i) {
        int yoffset = i * width * channels;
        for (int j = 0; j <= width - ksize; ++j) {
            int xoffset = yoffset + j * channels;
            ConvUInt8((src + xoffset), (tempOut + xoffset), width, channels, filter, ksize);
        }
    }

}

void ImageConvolution(
    float* src, int width, int height, int channels,
    float* dst, float* filter, int ksize){

    size_t size = (size_t)width * (size_t)height * sizeof(float) * channels;

    int startX = ksize / 2;
    int endX = width - ksize / 2;

    int startY = ksize / 2;
    int endY = height - ksize / 2;

    float* tempOut = dst + (startY * width + startX) * 3;

    memset(dst, 0, size);
    
    for (int i = 0; i <= height - ksize; ++i) {
        int yoffset = i * width * channels;
        for (int j = 0; j <= width - ksize; ++j) {
            int xoffset = yoffset + j * channels;
            ConvFloat((src + xoffset), (tempOut + xoffset), width, channels, filter, ksize);
        }
    }
}


int dichotomy(unsigned char* A, int start, int end){
    int banchmark = A[start];

    int nLeft = start;

    for (int i = start + 1; i < end; ++i) {
        if (banchmark > A[i]) {
            nLeft++;
            std::swap(A[i], A[nLeft]);
        }
    }
    std::swap(A[nLeft], A[start]);
    
    return nLeft;
}

int dichotomy(float* A, int start, int end){
    int banchmark = A[start];

    int nLeft = start;

    for (int i = start + 1; i < end; ++i) {
        if (banchmark > A[i]) {
            nLeft++;
            std::swap(A[i], A[nLeft]);
        }
    }
    std::swap(A[nLeft], A[start]);
    
    return nLeft;
}

unsigned char quickSelect(unsigned char* A, int start, int end, int n) {
    int small = dichotomy(A, start, end);
    if (small == n - 1) {
        return A[small];
    }
    else if(small > (n - 1)){
        quickSelect(A, start, small, n);
    }
    else {
        quickSelect(A, small + 1, end, n);
    }
}

float quickSelect(float* A, int start, int end, int n) {
    int small = dichotomy(A, start, end);
    if (small == n - 1) {
        return A[small];
    }
    else if(small > (n - 1)){
        quickSelect(A, start, small, n);
    }
    else {
        quickSelect(A, small + 1, end, n);
    }
}

void ImageMedian(unsigned char* src, int width, int height, int channels, unsigned char* dst, int ksize){
    
    int s = -ksize / 2;
    int e = ksize / 2;

    for(int i = 0; i < height; ++ i){

        for(int j = 0; j < width; ++ j){

            
            int starty = (i + s) < 0 ? (-i) : (s);
            int startx = (j + s) < 0 ? (-j) : (s);
            int endy = (i + e) > (height - 1) ? (height - 1 - i) : (e);
            int endx = (j + e) > (width - 1) ? (width - 1 - j) : (e);

            for(int c = 0; c < channels; ++ c){
                std::vector<unsigned char> vec;

                for(int m = starty; m <= endy; ++ m){
                    for(int n = startx; n <= endx; ++ n){
                        vec.push_back(src[((i + m) * width + (j + n)) * channels + c]);
                    }
                }
                dst[(i * width + j) * channels + c] = quickSelect(vec.data(), 0, vec.size(), vec.size() / 2);
            }
        }
    }
}

void ImageMedian(float* src, int width, int height, int channels, float* dst, int ksize){
    int s = -ksize / 2;
    int e = ksize / 2;

    for(int i = 0; i < height; ++ i){

        for(int j = 0; j < width; ++ j){

            
            int starty = (i + s) < 0 ? (-i) : (s);
            int startx = (j + s) < 0 ? (-j) : (s);
            int endy = (i + e) > (height - 1) ? (height - 1 - i) : (e);
            int endx = (j + e) > (width - 1) ? (width - 1 - j) : (e);

            for(int c = 0; c < channels; ++ c){
                std::vector<unsigned char> vec;

                for(int m = starty; m <= endy; ++ m){
                    for(int n = startx; n <= endx; ++ n){
                        vec.push_back(src[((i + m) * width + (j + n)) * channels + c]);
                    }
                }
                dst[(i * width + j) * channels + c] = quickSelect(vec.data(), 0, vec.size(), vec.size() / 2);
            }
        }
    }
}

void ImageAvg(unsigned char* src, int width, int height, int channels, unsigned char* dst, int ksize){
    
    int s = -ksize / 2;
    int e = ksize / 2;
    for(int i = 0; i < height; ++ i){
        for(int j = 0; j < width; ++ j){
            
            int starty = (i + s) < 0 ? (-i) : (s);
            int startx = (j + s) < 0 ? (-j) : (s);
            int endy = (i + e) > (height - 1) ? (height - 1 - i) : (e);
            int endx = (j + e) > (width - 1) ? (width - 1 - j) : (e);

            int size = (endy - starty + 1) * (endx - startx + 1);

            for(int c = 0; c < channels; ++ c){
                int sum = 0;

                for(int m = starty; m <= endy; ++ m){
                    for(int n = startx; n <= endx; ++ n){
                        sum += src[((i + m) * width + (j + n)) * channels + c];
                    }
                }
                dst[(i * width + j) * channels + c] = (unsigned char)roundf(sum * 1.f / size);
            }

        }
    }
}

void ImageAvg(float* src, int width, int height, int channels, float* dst, int ksize){
    int s = -ksize / 2;
    int e = ksize / 2;
    for(int i = 0; i < height; ++ i){
        for(int j = 0; j < width; ++ j){
            
            int starty = (i + s) < 0 ? (-i) : (s);
            int startx = (j + s) < 0 ? (-j) : (s);
            int endy = (i + e) > (height - 1) ? (height - 1 - i) : (e);
            int endx = (j + e) > (width - 1) ? (width - 1 - j) : (e);

            int size = (endy - starty + 1) * (endx - startx + 1);

            for(int c = 0; c < channels; ++ c){
                int sum = 0;

                for(int m = starty; m <= endy; ++ m){
                    for(int n = startx; n <= endx; ++ n){
                        sum += src[((i + m) * width + (j + n)) * channels + c];
                    }
                }
                dst[(i * width + j) * channels + c] = (unsigned char)roundf(sum * 1.f / size);
            }

        }
    }
}

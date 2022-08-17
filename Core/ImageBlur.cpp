#include <math.h>
#include <vector>
#include <memory.h>
#include <time.h>
#include "Common.h"
#include "ImageBlur.h"

static int dichotomy(unsigned char* A, int start, int end){
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

static int dichotomy(float* A, int start, int end){
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

static unsigned char quickSelect(unsigned char* A, int start, int end, int n) {
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

static float quickSelect(float* A, int start, int end, int n) {
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

void ImageGlass(unsigned char* src, int width, int height, int channels, unsigned char* dst, int ksize){

    srand(time(NULL));

    int s = -ksize / 2;
    int e = ksize / 2;
    for(int i = 0; i < height; ++ i){
        for(int j = 0; j < width; ++ j){
            
            int starty = (i + s) < 0 ? (-i) : (s);
            int startx = (j + s) < 0 ? (-j) : (s);
            int endy = (i + e) > (height - 1) ? (height - 1 - i) : (e);
            int endx = (j + e) > (width - 1) ? (width - 1 - j) : (e);

            int y = rand() % (endy - starty + 1) + starty;
            int x = rand() % (endx - startx + 1) + startx;

            for(int c = 0; c < channels; ++ c){
                dst[(i * width + j) * channels + c] = src[((i + y) * width + (j + x)) * channels + c];
            }

        }
    }
}

void ImageGlass(float* src, int width, int height, int channels, float* dst, int ksize){
    srand(time(NULL));

    int s = -ksize / 2;
    int e = ksize / 2;
    for(int i = 0; i < height; ++ i){
        for(int j = 0; j < width; ++ j){
            
            int starty = (i + s) < 0 ? (-i) : (s);
            int startx = (j + s) < 0 ? (-j) : (s);
            int endy = (i + e) > (height - 1) ? (height - 1 - i) : (e);
            int endx = (j + e) > (width - 1) ? (width - 1 - j) : (e);

            int y = rand() % (endy - starty + 1) + starty;
            int x = rand() % (endx - startx + 1) + startx;

            for(int c = 0; c < channels; ++ c){
                dst[(i * width + j) * channels + c] = src[((i + y) * width + (j + x)) * channels + c];
            }

        }
    }
}

static void convBilateral(float* in, float* out, int width, float* filter, int ksize, float* colorFilter, float* centerValue, int channels) {

    int lineSize = width * channels;
    float* inTemp = in;
    std::vector<float> sum(channels, 0.f);
    std::vector<float> filterSum(channels, 0.f);
    
    for (int i = 0; i < ksize; ++i) {
        for (int j = 0; j < ksize; ++j) {
            int xoffset = j * channels;
            for(int c = 0; c < channels; ++ c){
                float v = inTemp[xoffset + c];
    
                float bv = *filter * colorFilter[(int)(fabsf((v - centerValue[c]) * 255.f) + 0.5f)];
    
                filterSum[c] += bv;
    
                sum[c] += bv * v;
            }
            filter++;
        }
        inTemp = inTemp + lineSize;
    }

    for(int c = 0; c < channels; ++ c){
        float v = sum[c] / filterSum[c];
        out[c] = CLAMP(v, 0.f, 1.f);
    }
}

static void convBilateral(unsigned char* in, unsigned char* out, int width, float* filter, int ksize, float* colorFilter, unsigned char* centerValue, int channels) {

    int lineSize = width * channels;
    unsigned char* inTemp = in;
    std::vector<float> sum(channels, 0.f);
    std::vector<float> filterSum(channels, 0.f);
    
    for (int i = 0; i < ksize; ++i) {
        for (int j = 0; j < ksize; ++j) {
            int xoffset = j * channels;
            for(int c = 0; c < channels; ++ c){
                float v = inTemp[xoffset + c];
    
                float bv = *filter * colorFilter[(int)(fabsf((v - centerValue[c])) + 0.5f)];
    
                filterSum[c] += bv;
    
                sum[c] += bv * v;
            }
            filter++;
        }
        inTemp = inTemp + lineSize;
    }

    for(int c = 0; c < channels; ++ c){
        float v = sum[c] / filterSum[c] + 0.5f;
        out[c] = (unsigned char)(CLAMP(v, 0.f, 255.f));
    }
}

void ImageBilaternal(unsigned char* src, int width, int height, int channels, unsigned char* dst, float* gaussFilter, int ksize, float* colorFilter){
    
    int startX = ksize / 2;
    int endX = width - ksize / 2;
    int startY = ksize / 2;
    int endY = height - ksize / 2;

    unsigned char* tmpOut = dst + (startY * width + startX) * channels;
    memset(dst, 0, width * height * channels * sizeof(unsigned char));

    for(int h = 0; h <= height - ksize; ++ h){
        int yoffset = h * width;
        for(int w = 0; w <= width - ksize; ++ w){
            int xoffset = (yoffset + w) * channels;
            int centerIndex = ((h + startY) * width + (w + startX)) * channels;
            convBilateral((src + xoffset), (tmpOut + xoffset), width, gaussFilter, ksize, colorFilter, (src + centerIndex), channels);
        }
    }
}

void ImageBilaternal(float* src, int width, int height, int channels, float* dst, float* gaussFilter, int ksize, float* colorFilter){
    
    int startX = ksize / 2;
    int endX = width - ksize / 2;
    int startY = ksize / 2;
    int endY = height - ksize / 2;

    float* tmpOut = dst + (startY * width + startX) * channels;
    memset(dst, 0, width * height * channels * sizeof(float));

    for(int h = 0; h <= height - ksize; ++ h){
        int yoffset = h * width;
        for(int w = 0; w <= width - ksize; ++ w){
            int xoffset = (yoffset + w) * channels;
            int centerIndex = ((h + startY) * width + (w + startX)) * channels;
            convBilateral((src + xoffset), (tmpOut + xoffset), width, gaussFilter, ksize, colorFilter, (src + centerIndex), channels);
        }
    }
}
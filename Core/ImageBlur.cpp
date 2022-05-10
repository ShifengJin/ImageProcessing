#include <math.h>
#include <vector>
#include <memory.h>
#include <time.h>
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

    int offset = ksize / 2;
    int lineSize = width * channels;
    unsigned char* inTemp = in;
    std::vector<float> sum(channels, 0.f);
    for (int i = -offset; i <= offset; ++i) {
        for (int j = -offset; j <= offset; ++j) {
            int index = (i * width + j) * channels;
            for(int c = 0; c < channels; ++ c){
                sum[c] += (*filter) * *(inTemp + index + c);
            }
            filter++;
        }
    }
    for(int c = 0; c < channels; ++ c){
        out[c] = CLAMP(sum[c], (unsigned char)0, (unsigned char)255);
    }
}

void ConvFloat(float* in, float* out, int width, int channels, float* filter, int ksize) {
    int offset = ksize / 2;
    int lineSize = width * channels;
    float* inTemp = in;
    std::vector<float> sum(channels, 0.f);
    for (int i = -offset; i <= offset; ++i) {
        for (int j = -offset; j <= offset; ++j) {
            int index = (i * width + j) * channels;
            for(int c = 0; c < channels; ++ c){
                sum[c] += (*filter) * *(inTemp + index + c);
            }
            filter++;
        }
    }
    for(int c = 0; c < channels; ++ c){
        //out[c] = CLAMP(sum[c], 0.f, 1.f);
        out[c] = sum[c];
    }
}

void ImageConvolution(
    unsigned char* src, int width, int height, int channels,
    unsigned char* dst, float* filter, int ksize){

    size_t size = (size_t)width * (size_t)height * sizeof(unsigned char) * channels;

    int offset = ksize / 2;

    int startX = offset;
    int endX = width - offset;

    int startY = offset;
    int endY = height - offset;
    
    memset(dst, 0, size);
    
     for (int i = offset; i < height - offset; ++i) {
        int yoffset = i * width * channels;
        for (int j = offset; j < width - offset; ++j) {
            int xoffset = yoffset + j * channels;
            ConvUInt8((src + xoffset), (dst + xoffset), width, channels, filter, ksize);
        }
    }
}

void ImageConvolution(
    float* src, int width, int height, int channels,
    float* dst, float* filter, int ksize){

    size_t size = (size_t)width * (size_t)height * sizeof(float) * channels;

    int offset = ksize / 2;

    int startX = offset;
    int endX = width - offset;

    int startY = offset;
    int endY = height - offset;
    
    memset(dst, 0, size);
    
    for (int i = offset; i < height - offset; ++i) {
        int yoffset = i * width * channels;
        for (int j = offset; j < width - offset; ++j) {
            int xoffset = yoffset + j * channels;
            ConvFloat((src + xoffset), (dst + xoffset), width, channels, filter, ksize);
        }
    }
}

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


void BilaternalColorFilter(float* ofilter, float sigma){
    float colorSigmaInv = 0.5f / (sigma * sigma);
    for(int i = 0; i < 256; ++ i){
        float colorDiff = expf(-(i * i) * colorSigmaInv);
        ofilter[i] = colorDiff;
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
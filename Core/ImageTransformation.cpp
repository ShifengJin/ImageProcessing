#include <memory.h>
#include <math.h>
#include "Common.h"
#include "ImageTransformation.h"

void Utily::GaussFilter(int ksize, float sigma, float* ofilter){
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

void Utily::MotionFilter(int ksize, float angle, float* ofilter){
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

void Utily::BilaternalColorFilter(float* ofilter, float sigma){
    float colorSigmaInv = 0.5f / (sigma * sigma);
    for(int i = 0; i < 256; ++ i){
        float colorDiff = expf(-(i * i) * colorSigmaInv);
        ofilter[i] = colorDiff;
    }
}

void Utily::ConvUInt8(unsigned char* in, unsigned char* out, int width, int channels, float* filter, int ksize) {

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

void Utily::ConvFloat(float* in, float* out, int width, int channels, float* filter, int ksize) {
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

void Utily::ImageConvolution(unsigned char* src, int width, int height, int channels, unsigned char* dst, float* filter, int ksize){
    size_t size = (size_t)width * (size_t)height * sizeof(unsigned char) * channels;

    int offset = ksize / 2;

    int startX = offset;
    int endX = width - offset;

    int startY = offset;
    int endY = height - offset;
    
    memset(dst, 0, size);
    
    //for (int i = offset; i < height - offset; ++i) {
    //    int yoffset = i * width * channels;
    //    for (int j = offset; j < width - offset; ++j) {
    //        int xoffset = yoffset + j * channels;
    //        ConvUInt8((src + xoffset), (dst + xoffset), width, channels, filter, ksize);
    //    }
    //}
    for (int i = 0; i < height; ++i) {
        int yoffset = i * width * channels;
        if(i < offset || i >= (height - offset)){
            memcpy(dst + yoffset, src + yoffset, width * height * channels * sizeof(unsigned char));
            continue;
        }
        for (int j = 0; j < width; ++j) {
            int xoffset = yoffset + j * channels;
            if(j < offset || j >= (width - offset)){
                *(dst + xoffset) = *(src + xoffset);
                continue;
            }
            ConvUInt8((src + xoffset), (dst + xoffset), width, channels, filter, ksize);
        }
    }
}

void Utily::ImageConvolution(float* src, int width, int height, int channels, float* dst, float* filter, int ksize){
    size_t size = (size_t)width * (size_t)height * sizeof(float) * channels;

    int offset = ksize / 2;

    int startX = offset;
    int endX = width - offset;

    int startY = offset;
    int endY = height - offset;
    
    memset(dst, 0, size);
    
    //for (int i = offset; i < height - offset; ++i) {
    //    int yoffset = i * width * channels;
    //    for (int j = offset; j < width - offset; ++j) {
    //        int xoffset = yoffset + j * channels;
    //        ConvFloat((src + xoffset), (dst + xoffset), width, channels, filter, ksize);
    //    }
    //}
    for (int i = 0; i < height; ++i) {
        int yoffset = i * width * channels;
        if(i < offset || i >= (height - offset)){
            memcpy(dst + yoffset, src + yoffset, width * height * channels * sizeof(float));
            continue;
        }
        for (int j = 0; j < width; ++j) {
            int xoffset = yoffset + j * channels;
            if(j < offset || j >= (width - offset)){
                *(dst + xoffset) = *(src + xoffset);
                continue;
            }
            ConvFloat((src + xoffset), (dst + xoffset), width, channels, filter, ksize);
        }
    }
}


void Utily::ImageConvolution1(float* src, int width, int height, int channels, float* dst, float* filter, int ksize){
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

void Utily::TwoImageDiff(float* img0, float* img1, int width, int height, int channels, float* out){
    int size = width * height * channels;
    for(int i = 0; i < size; ++i){
        out[i] = img0[i] - img1[i];
    }
}

void Utily::RGB2Gray(unsigned char* rgb, unsigned char* gray, int width, int height){
    int size = width * height;
    unsigned char* inRgb = rgb;
    unsigned char* outGray = gray;
    for(int i = 0; i < size; ++ i){
        unsigned char r = *inRgb;
        unsigned char g = *(inRgb + 1);
        unsigned char b = *(inRgb + 2);
        *outGray = (unsigned char)CLAMP(CVT_Y_255(r, g, b), (unsigned char)0, (unsigned char)255);
        
        inRgb += 3;
        outGray += 1;
    }
}

void Utily::RGB2Gray(unsigned char* rgb, float* gray, int width, int height){
    int size = width * height;
    unsigned char* inRgb = rgb;
    float* outGray = gray;
    for(int i = 0; i < size; ++ i){
        unsigned char r = *inRgb;
        unsigned char g = *(inRgb + 1);
        unsigned char b = *(inRgb + 2);
        *outGray = CLAMP(CVT_Y_255(r, g, b), 0.f, 255.f);
        
        inRgb += 3;
        outGray += 1;
    }
}

void Utily::RGB2Gray(float* rgb, float* gray, int width, int height){
    int size = width * height;
    float * inRgb = rgb;
    float * outGray = gray;
    for(int i = 0; i < size; ++ i){
        float r = *inRgb;
        float g = *(inRgb + 1);
        float b = *(inRgb + 2);
        *outGray = CLAMP(CVT_Y_Normal(r, g, b), 0.f, 1.f);
        
        inRgb += 3;
        outGray += 1;
    }
}

void DrawPointToImageRGB(unsigned char* rgb, unsigned char* oRgb, int width, int height, Color_UChar color, std::vector<Coordinate_i> coordinates){
    if(rgb != oRgb)
        memcpy(oRgb, rgb, sizeof(unsigned char) * width * height * 3);

    for(int i = 0; i < coordinates.size(); ++ i){
        
        int x = coordinates[i].x;
        int y = coordinates[i].y;

        if(y - 1 >= 0 && x >= 0 && y - 1 < height && x < width){
            oRgb[((y - 1) * width + x) * 3 + 0]  = color.r;
            oRgb[((y - 1) * width + x) * 3 + 1]  = color.g;
            oRgb[((y - 1) * width + x) * 3 + 2]  = color.b;
        }

        if(y >= 0 && x - 1 >= 0 && y < height && x - 1 < width){
            oRgb[(y * width + x - 1) * 3 + 0]  = color.r;
            oRgb[(y * width + x - 1) * 3 + 1]  = color.g;
            oRgb[(y * width + x - 1) * 3 + 2]  = color.b;
        }

        if(y >= 0 && x >= 0 && y < height && x < width){
            oRgb[(y * width + x) * 3 + 0]  = color.r;
            oRgb[(y * width + x) * 3 + 1]  = color.g;
            oRgb[(y * width + x) * 3 + 2]  = color.b;
        }

        if(y >= 0 && x + 1 >= 0 && y < height && x + 1 < width){
            oRgb[(y * width + x + 1) * 3 + 0]  = color.r;
            oRgb[(y * width + x + 1) * 3 + 1]  = color.g;
            oRgb[(y * width + x + 1) * 3 + 2]  = color.b;
        }

        if(y + 1 >= 0 && x >= 0 && y + 1 < height && x < width){
            oRgb[((y + 1) * width + x) * 3 + 0]  = color.r;
            oRgb[((y + 1) * width + x) * 3 + 1]  = color.g;
            oRgb[((y + 1) * width + x) * 3 + 2]  = color.b;
        }
    }
}

void DrawPointToImageGray(unsigned char* gray, unsigned char* oGray, int width, int height, unsigned char color, std::vector<Coordinate_i> coordinates){
    if(gray != oGray)
        memcpy(oGray, gray, sizeof(unsigned char) * width * height);

    for(int i = 0; i < coordinates.size(); ++ i){
        
        int x = coordinates[i].x;
        int y = coordinates[i].y;

        if(y - 1 >= 0 && x >= 0 && y - 1 < height && x < width){
            oGray[((y - 1) * width + x) + 0]  = color;
        }

        if(y >= 0 && x - 1 >= 0 && y < height && x - 1 < width){
            oGray[(y * width + x - 1) + 0]  = color;
        }

        if(y >= 0 && x >= 0 && y < height && x < width){
            oGray[(y * width + x) + 0]  = color;
        }

        if(y >= 0 && x + 1 >= 0 && y < height && x + 1 < width){
            oGray[(y * width + x + 1) + 0]  = color;
        }

        if(y + 1 >= 0 && x >= 0 && y + 1 < height && x < width){
            oGray[((y + 1) * width + x) + 0]  = color;
        }
    }
}
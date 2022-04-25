#include <math.h>
#include <vector>
#include "Common.h"
#include "ImageRotate.h"

void ImageRotate(unsigned char* src, int width, int height, int channels,
                 unsigned char* dst, float angle){

    float degAngle = angle * ANGLE2REG;
    float cosAngle = cosf(degAngle);
    float sinAngle = sinf(degAngle);
    float negativeSinAngle = -sinAngle;

    // cos    -sin    Inv    cos    sin
    // sin     cos          -sin    cos

    float centerX = width / 2.f;
    float centerY = height / 2.f;

    for(int i = 0; i < height; ++ i){

        float oY = i - centerY;
        for(int j = 0; j < width; ++ j){
            float oX = j - centerX;
            
            float iX = cosAngle * oX + sinAngle * oY + centerX;
            float iY = negativeSinAngle * oX + cosAngle * oY + centerY;

            if(iX < 0.f || iX > (width - 1.f) || iY < 0.f || iY > (height - 1.f)){
                for(int c = 0; c < channels; ++ c){
                    dst[(i * width + j) * channels + c] = (unsigned char)0;
                }
                continue;
            }

            int UpY = (int)iY;
            int DownY = UpY + 1;
            int LeftX = (int)iX;
            int RightX = LeftX + 1;

            float UpWeight = DownY - iY;
            float DownWeight = 1.f - UpWeight;
            float LeftWeight = RightX - iX;
            float RightWeight = 1.f - LeftWeight;

            std::vector<float> sums(channels, 0);
            for(int c = 0; c < channels; ++ c){
                sums[c] += 
                (src[(UpY * width + LeftX) * channels + c] * LeftWeight + src[(UpY * width + RightX) * channels + c] * RightWeight) * UpWeight + 
                (src[(DownY * width + LeftX) * channels + c] * LeftWeight + src[(DownY * width + RightX) * channels + c] * RightWeight) * DownWeight;
            }
            for(int c = 0; c < channels; ++ c){                
                dst[(i * width + j) * channels + c] = CLAMP(sums[c], (unsigned char)0, (unsigned char)255);
            }

        }
    }

}

void ImageRotate(float* src, int width, int height, int channels,
                 float* dst, float angle){
    
    float degAngle = angle * ANGLE2REG;
    float cosAngle = cosf(degAngle);
    float sinAngle = sinf(degAngle);
    float negativeSinAngle = -sinAngle;

    // cos    -sin    Inv    cos    sin
    // sin     cos          -sin    cos

    float centerX = width / 2.f;
    float centerY = height / 2.f;

    for(int i = 0; i < height; ++ i){

        float oY = i - centerY;
        for(int j = 0; j < width; ++ j){
            float oX = j - centerX;
            
            float iX = cosAngle * oX + sinAngle * oY + centerX;
            float iY = negativeSinAngle * oX + cosAngle * oY + centerY;

            if(iX < 0.f || iX > (width - 1.f) || iY < 0.f || iY > (height - 1.f)){
                for(int c = 0; c < channels; ++ c){
                    dst[(i * width + j) * channels + c] = (unsigned char)0;
                }
                continue;
            }

            int UpY = (int)iY;
            int DownY = UpY + 1;
            int LeftX = (int)iX;
            int RightX = LeftX + 1;

            float UpWeight = DownY - iY;
            float DownWeight = 1.f - UpWeight;
            float LeftWeight = RightX - iX;
            float RightWeight = 1.f - LeftWeight;

            std::vector<float> sums(channels, 0);
            for(int c = 0; c < channels; ++ c){
                sums[c] += 
                (src[(UpY * width + LeftX) * channels + c] * LeftWeight + src[(UpY * width + RightX) * channels + c] * RightWeight) * UpWeight + 
                (src[(DownY * width + LeftX) * channels + c] * LeftWeight + src[(DownY * width + RightX) * channels + c] * RightWeight) * DownWeight;
            }
            for(int c = 0; c < channels; ++ c){                
                dst[(i * width + j) * channels + c] = sums[c];
            }

        }
    }
}
#include <memory.h>
#include <malloc.h>
#include <math.h>
#include <iostream>
#include <vector>
#include "Common.h"
#include "ImageFastCorner.h"

static bool UnSuppression(float* socre, int width, int height, int x, int y, float value, int unSuppressionRadius){
    
    int startY = y - unSuppressionRadius;
    startY = startY < 0 ? 0 : startY;
    int endY = y + unSuppressionRadius;
    endY = endY > (height - 1) ? (height - 1) : endY;

    int startX = x - unSuppressionRadius;
    startX = startX < 0 ? 0 : startX;
    int endX = x + unSuppressionRadius;
    endX = endX > (width - 1) ? (width - 1) : endX;

    for(int i = startY; i <= endY; ++ i){
        for(int j = startX; j <= endX; ++ j){
            if(i == y && j == x) continue;
            if(socre[i * width + j] >= value){
                return false;
            }
        }
    }
    return true;
}

float cornerScore(const float* ptr, const int pixel[], int threshold, int number){
    
    float score = 0.f;
    float v = ptr[0];
    for(int k = 0; k < number; ++k){
        score += fabsf((v - ptr[pixel[k]]));
    }
    return score;
}

void FastCorner_16_9(float* gray, int width, int height, std::vector<Vector2i>& oCornerCoordiantes, float threshold, bool nonmax_suppression, int unSuppressionRadius, float* score){
    const int patternSize = 16;
    const int K = patternSize / 2;
    const int N = patternSize + K + 1;

    const int offsets16[16][2] = {
        {0,  3}, { 1,  3}, { 2,  2}, { 3,  1}, { 3, 0}, { 3, -1}, { 2, -2}, { 1, -3},
        {0, -3}, {-1, -3}, {-2, -2}, {-3, -1}, {-3, 0}, {-3,  1}, {-2,  2}, {-1,  3}
    };

    int pixel[25] = { 0 };
    
    for(int i = 0; i < patternSize; ++ i){
        pixel[i] = offsets16[i][0] + offsets16[i][1] * width;
    }
    for(int i = patternSize; i < 25; ++i){
        pixel[i] = pixel[i - patternSize];
    }

    unsigned char threshold_tab[512];
    for(int i = -255; i <= 255; ++ i){
        float tmp = i / 255.f;
        threshold_tab[i + 255] = (unsigned char)(tmp < -threshold ? 1 : (tmp > threshold ? 2 : 0));
    }
    std::vector<Vector2i> tmpCornerCoordiantes;
    for(int i = 3; i < height - 3; ++ i){
        const float* ptr = gray + i * width + 3;
        int ncorners = 0;
        
        for(int j = 3; j < width - 3; ++j, ptr++){

            float pixelValue = ptr[0];
            int v = (int)(pixelValue * 255.f + 0.5f);
            const unsigned char* tab = &threshold_tab[0] - v + 255;
            int pixel0 = (int)(ptr[pixel[0]] * 255.f + 0.5f);
            int pixel8 = (int)(ptr[pixel[8]] * 255.f + 0.5f);
            int d = tab[pixel0] | tab[pixel8];
            if(d == 0) continue;
            
            int pixel2 = (int)(ptr[pixel[2]] * 255.f + 0.5f);
            int pixel10 = (int)(ptr[pixel[10]] * 255.f + 0.5f);
            d &= (tab[pixel2] | tab[pixel10]);

            int pixel4 = (int)(ptr[pixel[4]] * 255.f + 0.5f);
            int pixel12 = (int)(ptr[pixel[12]] * 255.f + 0.5f);
            d &= (tab[pixel4] | tab[pixel12]);
            
            int pixel6 = (int)(ptr[pixel[6]] * 255.f + 0.5f);
            int pixel14 = (int)(ptr[pixel[14]] * 255.f + 0.5f);
            d &= (tab[pixel6] | tab[pixel14]);

            if(d == 0) continue;

            int pixel1 = (int)(ptr[pixel[1]] * 255.f + 0.5f);
            int pixel9 = (int)(ptr[pixel[9]] * 255.f + 0.5f);
            d &= (tab[pixel1] | tab[pixel9]);

            int pixel3 = (int)(ptr[pixel[3]] * 255.f + 0.5f);
            int pixel11 = (int)(ptr[pixel[11]] * 255.f + 0.5f);
            d &= (tab[pixel3] | tab[pixel11]);

            int pixel5 = (int)(ptr[pixel[5]] * 255.f + 0.5f);
            int pixel13 = (int)(ptr[pixel[13]] * 255.f + 0.5f);
            d &= (tab[pixel5] | tab[pixel13]);

            int pixel7 = (int)(ptr[pixel[7]] * 255.f + 0.5f);
            int pixel15 = (int)(ptr[pixel[15]] * 255.f + 0.5f);
            d &= (tab[pixel7] | tab[pixel15]);

            if(d & 1){
                float vt = pixelValue - threshold;
                int count = 0;
                for(int m = 0; m < N; m ++){
                    float x = ptr[pixel[m]];
                    if(x < vt){
                        if(++count > K){                            
                            if(nonmax_suppression){
                                score[i * width + j] = cornerScore(ptr, pixel, threshold, patternSize);
                                tmpCornerCoordiantes.push_back({j, i});
                            }else{
                                oCornerCoordiantes.push_back({j, i});
                            }
                            break;
                        }
                    }else{
                        count = 0;
                    }
                }
            }

            if(d & 2){
                float vt = pixelValue + threshold;
                int count = 0;
                for(int m = 0; m < N; m++){
                    float x = ptr[pixel[m]];
                    if(x > vt){
                        if(++count > K){
                            if(nonmax_suppression){
                                score[i * width + j] = cornerScore(ptr, pixel, threshold, patternSize);
                                tmpCornerCoordiantes.push_back({j, i});
                            }else{
                                oCornerCoordiantes.push_back({j, i});
                            }
                            break;
                        }
                    }else{
                        count = 0;
                    }
                }
            }
        }

    }
    
    if(nonmax_suppression){
        for(int i = 0; i < tmpCornerCoordiantes.size(); ++ i){
            int x = tmpCornerCoordiantes[i].x;
            int y = tmpCornerCoordiantes[i].y;
    
            if(UnSuppression(score, width, height, x, y, score[y * width + x], unSuppressionRadius)){
                oCornerCoordiantes.push_back({x, y});
            }
        }
    }
}
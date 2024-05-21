#include <math.h>
#include <iostream>
#include "ImageSusanCorner.h"

static int SusanMask(std::vector<std::vector<bool>>& mask, int maskRadius){
    int size = 2 * maskRadius + 1;
    int sum = 0;
    int centerX = maskRadius;
    int centerY = maskRadius;
    int radius2 = maskRadius * maskRadius;
    for(int i = 0; i < size; ++ i){
        int y = (i - centerY) * (i - centerY);
        for(int j = 0; j < size; ++ j){
            int x = (j - centerX) * (j - centerX);
            if(y + x <= radius2){
                sum ++;
                mask[i][j] = true;
            }
        }
    }
    return sum;
}

static int USAN(float value, float* gray, int width, std::vector<std::vector<bool>> mask, int maskRadius, float threshold){
    int sum = 0;
    for(int i = -maskRadius; i <= maskRadius; ++ i){
        for(int j = -maskRadius; j <= maskRadius; ++ j){
            float v = fabsf(*(gray + (i * width) + j) - value);
            if(mask[i + maskRadius][j + maskRadius] && v < threshold){
                sum ++;
            }
        }
    }
    return sum;
}

static bool UnSuppression(float* tmpR, int width, int height, int x, int y, float value, int unSuppressionRadius){
    
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
            if(tmpR[i * width + j] >= value){
                return false;
            }
        }
    }
    return true;
}

void SusanCorner(float* gray, int width, int height, std::vector<Vector2i>& oCornerCoordiantes, int maskRadius, int unSuppressionRadius, float threshold, float* tmpR){
    std::vector<Vector2i> tmpCornerCoordiantes;
    // make mask
    int size = 2 * maskRadius + 1;
    std::vector<std::vector<bool>> mask(size, std::vector<bool>(size, false));
    int maskNum = SusanMask(mask, maskRadius);
    //float th2 = maskNum / 2;    
    float th2 = maskNum / 5 * 4;    

    for(int y = maskRadius; y < height - maskRadius; ++ y){
        for(int x = maskRadius; x < width - maskRadius; ++ x){
            int n = USAN(*(gray + y * width + x), gray + y * width + x, width, mask, maskRadius, threshold);
            if(n < th2){
                *(tmpR + y * width + x) = th2 - n;
                tmpCornerCoordiantes.push_back({x, y});                
            }
        }
    }

    for(int i = 0; i < tmpCornerCoordiantes.size(); ++ i){
        int x = tmpCornerCoordiantes[i].x;
        int y = tmpCornerCoordiantes[i].y;
        
        if(UnSuppression(tmpR, width, height, x, y, tmpR[y * width + x], unSuppressionRadius)){
            oCornerCoordiantes.push_back({x, y});
            //std::cout << x << ", " << y << std::endl;
        }
    }
    tmpCornerCoordiantes.clear();
}
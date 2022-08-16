#include <memory.h>
#include "Common.h"
#include "ImageTransformation.h"

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
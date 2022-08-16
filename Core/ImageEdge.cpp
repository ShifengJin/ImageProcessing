#include <string.h>
#include <math.h>
#include "ImageEdge.h"

static void ImageL1(float* edgeX, float* edgeY, int width, int height, float* out){
    memset(out, 0, width * height * sizeof(float));
    int size = width * height;
    for(int i = 0; i < size; ++ i){
        out[i] = fabs(edgeX[i]) + fabs(edgeY[i]);
    }
}

static void ImageL2(float* edgeX, float* edgeY, int width, int height, float* out){
    memset(out, 0, width * height * sizeof(float));
    int size = width * height;
    for(int i = 0; i < size; ++ i){
        float x = edgeX[i];
        float y = edgeY[i];
        out[i] = sqrtf(x * x + y * y);
    }
}

static void ImageLInfinite(float* edgeX, float* edgeY, int width, int height, float* out){
    memset(out, 0, width * height * sizeof(float));
    int size = width * height;
    for(int i = 0; i < size; ++ i){
        float x = fabs(edgeX[i]);
        float y = fabs(edgeY[i]);
        out[i] = x > y ? x : y;
    }
}

void Sobel::Run(float* inGray, float* out, MAGTYPE magType){
    memset(m_edgeX, 0, m_width * m_height * sizeof(float));
    memset(m_edgeY, 0, m_width * m_height * sizeof(float));

    for(int y = 1; y < m_height - 1; ++ y){
        for(int x = 1; x < m_width - 1; ++ x){
            m_edgeX[y * m_width + x] = (inGray[(y - 1) * m_width + (x + 1)] + 2.f * inGray[y * m_width + (x + 1)] + inGray[(y + 1) * m_width + (x + 1)]) - 
                                       (inGray[(y - 1) * m_width + (x - 1)] + 2.f * inGray[y * m_width + (x - 1)] + inGray[(y + 1) * m_width + (x - 1)]);
            
            m_edgeY[y * m_width + y] = (inGray[(y + 1) * m_width + (x - 1)] + 2.f * inGray[(y + 1) * m_width + x] + inGray[(y + 1) * m_width + (x + 1)]) - 
                                       (inGray[(y - 1) * m_width + (x - 1)] + 2.f * inGray[(y - 1) * m_width + x] + inGray[(y - 1) * m_width + (x + 1)]);
        }
    }
    switch (magType){
        case MAG_L1:
            ImageL1(m_edgeX, m_edgeY, m_width, m_height, out);
            break;
        case MAG_L2:
            ImageL2(m_edgeX, m_edgeY, m_width, m_height, out);
            break;
        case MAG_LInfinite:
            ImageLInfinite(m_edgeX, m_edgeY, m_width, m_height, out);
            break;
    }

}

void Roberts::Run(float* inGray, float* out, MAGTYPE magType){
    memset(m_edgeX, 0, m_width * m_height * sizeof(float));
    memset(m_edgeY, 0, m_width * m_height * sizeof(float));

    for(int y = 0; y < m_height - 1; ++ y){
        for(int x = 0; x < m_width - 1; ++ x){
            m_edgeX[y * m_width + x] = inGray[y * m_width + x] - inGray[(y + 1) * m_width + x + 1];
            m_edgeY[y * m_width + x] = inGray[y * m_width + x + 1] - inGray[(y + 1) * m_width + x];
        }
    }
    switch (magType){
        case MAG_L1:
            ImageL1(m_edgeX, m_edgeY, m_width, m_height, out);
            break;
        case MAG_L2:
            ImageL2(m_edgeX, m_edgeY, m_width, m_height, out);
            break;
        case MAG_LInfinite:
            ImageLInfinite(m_edgeX, m_edgeY, m_width, m_height, out);
            break;
    }
}

void Prewitt::Run(float* inGray, float* out, MAGTYPE magType){
    memset(m_edgeX, 0, m_width * m_height * sizeof(float));
    memset(m_edgeY, 0, m_width * m_height * sizeof(float));

    for(int y = 1; y < m_height - 1; ++ y){
        for(int x = 1; x < m_width - 1; ++ x){
            m_edgeX[y * m_width + x] = (inGray[(y - 1) * m_width + (x + 1)] + inGray[y * m_width + (x + 1)] + inGray[(y + 1) * m_width + (x + 1)]) - 
                                       (inGray[(y - 1) * m_width + (x - 1)] + inGray[y * m_width + (x - 1)] + inGray[(y + 1) * m_width + (x - 1)]);
            
            m_edgeY[y * m_width + y] = (inGray[(y + 1) * m_width + (x - 1)] + inGray[(y + 1) * m_width + x] + inGray[(y + 1) * m_width + (x + 1)]) - 
                                       (inGray[(y - 1) * m_width + (x - 1)] + inGray[(y - 1) * m_width + x] + inGray[(y - 1) * m_width + (x + 1)]);
        }
    }
    switch (magType){
        case MAG_L1:
            ImageL1(m_edgeX, m_edgeY, m_width, m_height, out);
            break;
        case MAG_L2:
            ImageL2(m_edgeX, m_edgeY, m_width, m_height, out);
            break;
        case MAG_LInfinite:
            ImageLInfinite(m_edgeX, m_edgeY, m_width, m_height, out);
            break;
    }
}

void Laplacian::Run(float* inGray, float* out, Laplacian::Type type){

    switch (type){
        case Laplacian::Type::X_0_Y_0:
            X_0_Y_0(inGray, out);
            break;
        case Laplacian::Type::X_45_Y_45:
            X_45_Y_45(inGray, out);
            break;
        case Laplacian::Type::X_0_45_Y_0_45:
            X_0_45_Y_0_45(inGray, out);
            break;
    }

}

void Laplacian::X_0_Y_0(float* inGray, float* out){
    for(int y = 1; y < m_height - 1; ++ y){
        for(int x = 1; x < m_width - 1; ++x){
            out[y * m_width + x] = fabs( inGray[(y - 1) * m_width + x] + 
                                         inGray[y * m_width + x - 1] + 
                                         inGray[y * m_width + x + 1] + 
                                         inGray[(y + 1) * m_width + x] - inGray[y * m_width + x] * 4.f);
        }
    }
}

void Laplacian::X_45_Y_45(float* inGray, float* out){
    for(int y = 1; y < m_height - 1; ++ y){
        for(int x = 1; x < m_width - 1; ++x){
            out[y * m_width + x] = fabs( inGray[(y - 1) * m_width + x - 1] + 
                                         inGray[(y - 1) * m_width + x + 1] + 
                                         inGray[(y + 1) * m_width + x - 1] +
                                         inGray[(y + 1) * m_width + x + 1] - inGray[y * m_width + x] * 4.f);
        }
    }
}

void Laplacian::X_0_45_Y_0_45(float* inGray, float* out){
    for(int y = 1; y < m_height - 1; ++ y){
        for(int x = 1; x < m_width - 1; ++x){
            out[y * m_width + x] = fabs( inGray[(y - 1) * m_width + x - 1] + 
                                         inGray[(y - 1) * m_width + x] + 
                                         inGray[(y - 1) * m_width + x + 1] + 
                                         inGray[y * m_width + x - 1] + 
                                         inGray[y * m_width + x + 1] +
                                         inGray[(y + 1) * m_width + x - 1] +
                                         inGray[(y + 1) * m_width + x] +
                                         inGray[(y + 1) * m_width + x + 1] - inGray[y * m_width + x] * 8.f);
        }
    }
}
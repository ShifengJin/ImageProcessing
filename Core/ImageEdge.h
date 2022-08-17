#ifndef _IMAGEEDGE_H_
#define _IMAGEEDGE_H_

#include <iostream>
#include <memory>
#include "Common.h"

typedef enum _EDGETYPE_{
    EDGE_SOBEL = 0,
    EDGE_ROBERTS = 1,
    EDGE_PREWITT = 2,
    EDGE_LAPLACIAN = 3,
    EDGE_CANNY = 4
}EDGETYPE;

typedef enum _MAGTYPE_{
    MAG_L1 = 0,
    MAG_L2 = 1,
    MAG_LInfinite = 2
}MAGTYPE;


class Sobel{
    /*
    \Delta_xf=f(i+1, j-1)+2f(i+1, j)+f(i+1, j+1)-(f(i-1, j-1)+2f(i-1, j)+f(i-1, j+1))
    \Delta_yf=f(i-1, j+1)+2f(i, j+1)+f(i+1, j+1)-(f(i-1, j-1)+2f(i, j+1)+f(i+1, j-1))
    */
public:
    typedef std::shared_ptr<Sobel> ptr;

    Sobel(int width, int height): m_width(width), m_height(height){
        m_edgeX = (float*)calloc(m_width * m_height, sizeof(float));
        m_edgeY = (float*)calloc(m_width * m_height, sizeof(float));
    }

    ~Sobel(){
        FREE_Memory(m_edgeX);
        FREE_Memory(m_edgeY);
    }

    void Run(float* inGray, float* out, MAGTYPE magType, float* oEdgeX = nullptr, float* oEdgeY = nullptr);

    int m_width, m_height;

    float* m_edgeX;
    float* m_edgeY;
};

class Roberts{
    /*
    \Delta_xf=f(i, j)-f(i+1, j+1)
    \Delta_yf=f(i+1, j)-f(i, j+1)
    */
public:
    typedef std::shared_ptr<Roberts> ptr;

    Roberts(int width, int height): m_width(width), m_height(height){
        m_edgeX = (float*)calloc(m_width * m_height, sizeof(float));
        m_edgeY = (float*)calloc(m_width * m_height, sizeof(float));
    }

    ~Roberts(){
        FREE_Memory(m_edgeX);
        FREE_Memory(m_edgeY);
    }

    void Run(float* inGray, float* out, MAGTYPE magType);

    int m_width, m_height;
    float* m_edgeX;
    float* m_edgeY;
};

class Prewitt{
    /*
    \Delta_xf=f(i+1, j-1)+f(i+1, j)+f(i+1, j+1)-(f(i-1, j-1)+f(i-1, j)+f(i-1, j+1))
    \Delta_yf=f(i-1, j+1)+f(i, j+1)+f(i+1, j+1)-(f(i-1, j-1)+f(i, j+1)+f(i+1, j-1))
    */
public:
    typedef std::shared_ptr<Prewitt> ptr;

    Prewitt(int width, int height): m_width(width), m_height(height){
        m_edgeX = (float*)calloc(m_width * m_height, sizeof(float));
        m_edgeY = (float*)calloc(m_width * m_height, sizeof(float));
    }

    ~Prewitt(){
        FREE_Memory(m_edgeX);
        FREE_Memory(m_edgeY);
    }

    void Run(float* inGray, float* out, MAGTYPE magType);

    int m_width, m_height;
    float* m_edgeX;
    float* m_edgeY;
};

class Laplacian{
    /*
    \Delta^2f = \frac{\partial^2f}{{\partial x}^2}+\frac{\partial^2f}{{\partial y}^2}
    */
public:
    typedef std::shared_ptr<Laplacian> ptr;
    enum class Type{
        //  0   -1    0
        // -1    4   -1
        //  0   -1    0
        X_0_Y_0 = 0,
        // -1    0    -1
        //  0    4     0
        // -1    0    -1             
        X_45_Y_45 = 1,
        // -1    -1    -1
        // -1     8    -1
        // -1    -1    -1
        X_0_45_Y_0_45 = 2
    };

    Laplacian(int width, int height): m_width(width), m_height(height){}
    
    void Run(float* inGray, float* out, Laplacian::Type type = Laplacian::Type::X_0_Y_0);

    int m_width, m_height;
private:
    
    void X_0_Y_0(float* inGray, float* out);
    void X_45_Y_45(float* inGray, float* out);
    void X_0_45_Y_0_45(float* inGray, float* out);

};

class Canny{
public:
    typedef std::shared_ptr<Canny> ptr;
    Canny(int width, int height): m_width(width), m_height(height){
        m_edgeX = (float*)calloc(m_width * m_height, sizeof(float));
        m_edgeY = (float*)calloc(m_width * m_height, sizeof(float));
        m_gaussImage = (float*)calloc(m_width * m_height, sizeof(float));
        m_edgeMag = (float*)calloc(m_width * m_height, sizeof(float));
        m_edgeMag_noMaxsup = (float*)calloc(m_width * m_height, sizeof(float));
        m_pSobel = Sobel::ptr(new Sobel(m_width, m_height));
    }

    void Run(float* in, float* out, float TH = 80.f, float TL = 20.f, int gaussR = 1, float sigma = 1.4f, MAGTYPE magType = MAG_L2);

    ~Canny(){
        FREE_Memory(m_edgeX);
        FREE_Memory(m_edgeY);
        FREE_Memory(m_gaussImage);
        FREE_Memory(m_edgeMag);
        FREE_Memory(m_edgeMag_noMaxsup);
        m_pSobel.reset();
    }

    int m_width, m_height;

private:
    void noMax(float* edgeMag, float* edgeX, float* edgeY, float* edgeMag_noMaxsup);
    void trace(float* edgeMag_noMaxsup, float* out, int x, int y, float TL);
    bool checkInRange(int x, int y, int width, int height){
        return (x >= 0 && x < width && y >= 0 && y < height);
    }
private:
    float* m_edgeX;
    float* m_edgeY;
    float* m_gaussImage;
    float* m_edgeMag;
    float* m_edgeMag_noMaxsup;

    float m_TH, m_TL;

    Sobel::ptr m_pSobel;
    
};

#endif
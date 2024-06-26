#ifndef _SIFT_H_
#define _SIFT_H_

#include <iostream>
#include <memory>
#include <memory.h>
#include <vector>
#include "Common.h"

class SIFT
{

#define SIFT_OCTAVE              3
#define SIFT_LEVELS              3
#define SIFT_IMG_BORDER          5
#define SIFT_ORI_HIST_BINS       36
#define SIFT_ALPHA               10.f
#define SIFT_EDGERESPONSE        12.1f   // (SIFT_ALPHA + 1) * (SIFT_ALPHA + 1) / SIFT_ALPHA
#define SIFT_RESPONSE_TH         1.7f    // keypoint response threshold
#define SIFT_EXTREMA_TH_PIXEL    2.56f   // SIFT_EXTREMA_TH * 256
#define SIFT_INIT_SIGMA          0.5f    // 0.5f
#define SIFT_GAUSS_SIGMA         1.6f
#define SIFT_GAUSS_KERNEL_RATIO  3
#define SIFT_MAX_INTERP_STEPS    5


#define SHOWEXTREMEPOINTLOG 1


public:
    class Feature
    {
    public:
        Feature(Feature &feat)
        {
            memcpy(m_feature, feat.m_feature, 32 * sizeof(float));
            m_dir              = feat.m_dir;
            m_octave           = feat.m_octave;
            m_level            = feat.m_level;
            m_subLevel         = feat.m_subLevel;
            m_o_s_coordiante   = feat.m_o_s_coordiante;
            m_coordiante       = feat.m_coordiante;
            m_subCoordiante    = feat.m_subCoordiante;
            m_pyramidLevel     = feat.m_pyramidLevel;
            m_pyramidScale     = feat.m_pyramidScale;
            m_octScale         = feat.m_octScale;
            m_response         = feat.m_response;
            m_extremePointType = feat.m_extremePointType;
        }

        Feature(const Feature &feat)
        {
            memcpy(m_feature, feat.m_feature, 32 * sizeof(float));
            m_dir              = feat.m_dir;
            m_octave           = feat.m_octave;
            m_level            = feat.m_level;
            m_subLevel         = feat.m_subLevel;
            m_o_s_coordiante   = feat.m_o_s_coordiante;
            m_coordiante       = feat.m_coordiante;
            m_subCoordiante    = feat.m_subCoordiante;
            m_pyramidLevel     = feat.m_pyramidLevel;
            m_pyramidScale     = feat.m_pyramidScale;
            m_octScale         = feat.m_octScale;
            m_response         = feat.m_response;
            m_extremePointType = feat.m_extremePointType;
        }

        Feature()
        {
            memset(m_feature, 0, 32 * sizeof(float));
            m_dir              = 0.f;
            m_octave           = 0.f;
            m_level            = 0.f;
            m_subLevel         = 0.f;
            m_o_s_coordiante.x = 0.f;
            m_o_s_coordiante.y = 0.f;
            m_coordiante.x     = 0.f;
            m_coordiante.y     = 0.f;
            m_subCoordiante.x  = 0.f;
            m_subCoordiante.y  = 0.f;
            m_pyramidLevel     = 0.f;
            m_pyramidScale     = 0.f;
            m_octScale         = 0.f;
            m_response         = 0.f;
            m_extremePointType = 0;
        }

        Feature &operator=(const Feature &feat)
        {
            memcpy(m_feature, feat.m_feature, 32 * sizeof(float));
            m_dir              = feat.m_dir;
            m_octave           = feat.m_octave;
            m_level            = feat.m_level;
            m_subLevel         = feat.m_subLevel;
            m_o_s_coordiante   = feat.m_o_s_coordiante;
            m_coordiante       = feat.m_coordiante;
            m_subCoordiante    = feat.m_subCoordiante;
            m_pyramidLevel     = feat.m_pyramidLevel;
            m_pyramidScale     = feat.m_pyramidScale;
            m_octScale         = feat.m_octScale;
            m_response         = feat.m_response;
            m_extremePointType = feat.m_extremePointType;
            return *this;
        }

        float    m_feature[32];
        float    m_dir; // main dir
        float    m_octave;
        float    m_level;
        float    m_subLevel;          // subS + sOffset;
        Vector2f m_o_s_coordiante;    // x,y   for pyramid
        Vector2f m_o_s_subCoordiante; // x,y   sub for pyramid
        Vector2f m_coordiante;        // x,y   (x,y) * scale + offset           for srcImgSize
        Vector2f m_subCoordiante;     // x,y   (x+xi,y+yi) * scale + offset     for srcImgSize
        float    m_pyramidLevel;      // o * m_level + subS + sOffset;
        float    m_pyramidScale;      // powf(2.f, m_pyramidLevel / m_level);
        float    m_octScale;          // powf(2.f, feature.m_subLevel / m_level);
        float    m_response;
        int      m_extremePointType; // 1 big  2 little  0 default
    };

    class GaussFilter{
    public:
        GaussFilter(int iKernelWidth, float iSigma);
        ~GaussFilter();
        int kernel_width;
        float sigma;
        float* kernel;
    };

public:
    typedef std::shared_ptr<SIFT> ptr;

    // 为了满足尺度变化的连续性
    SIFT(unsigned int width, unsigned int height, unsigned int maxNum = 800, unsigned int Octave = SIFT_OCTAVE, unsigned int Level = SIFT_LEVELS, float sigma = SIFT_GAUSS_SIGMA);

    void Run(float *gray);

    ~SIFT();

    std::vector<Feature> m_Coordiantes;

private:
    void GaussPyramidImgs(float* gray);
    void DOGPyramidImgs();
    void FindExtremePoint();

    bool AdjustLocalExtrema(int width, int height, int x, int y, int o, int l, int extremePointType, float scale, float offset, Feature& oFeature);

private:
    unsigned int m_width, m_height, m_octave, m_level, m_maxNum;
    float m_sigma;

    std::vector<float> m_L_Sigma;
    std::vector<GaussFilter*> m_Level_GaussFilters;
    
    float *m_srcImg;

    std::vector<ImgSize> m_O_Size;
    std::vector<std::vector<float *>> m_O_L_GaussImgs; // img pyramid   Octave * (Scale + 3)

    std::vector<std::vector<float *>> m_O_L_DOGImgs;   // DOG pyramid Octave * (Scale + 2)

#if SHOWEXTREMEPOINTLOG
    int findNum = 0;
    int NGPixelThresholdNum = 0;
    int extremeNum = 0;
    int bigExtremeNum = 0;
    int littleExtremeNum = 0;

    int outBorderNum = 0;
    int DetHInvWroundNum = 0;
    int xOffsetOutNum = 0;
    int yOffsetOutNum = 0;
    int sOffsetOutNum = 0;
    int iterOutNum = 0;
    int responseOutNum = 0;
    int detHOutNum = 0;
    int edgeOutNum = 0;
#endif


};

#endif
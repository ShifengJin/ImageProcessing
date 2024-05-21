#ifndef _SIFT_H_
#define _SIFT_H_

#include <iostream>
#include <memory>
#include <memory.h>
#include <vector>
#include "Common.h"

class SIFT
{

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
            m_o_s_coordiante   = feat.m_o_s_coordiante;
            m_coordiante       = feat.m_coordiante;
            m_subCoordiante    = feat.m_subCoordiante;
            m_response         = feat.m_response;
            m_extremePointType = feat.m_extremePointType;
        }

        Feature(const Feature &feat)
        {
            memcpy(m_feature, feat.m_feature, 32 * sizeof(float));
            m_dir              = feat.m_dir;
            m_octave           = feat.m_octave;
            m_level            = feat.m_level;
            m_o_s_coordiante   = feat.m_o_s_coordiante;
            m_coordiante       = feat.m_coordiante;
            m_subCoordiante    = feat.m_subCoordiante;
            m_response         = feat.m_response;
            m_extremePointType = feat.m_extremePointType;
        }

        Feature()
        {
            memset(m_feature, 0, 32 * sizeof(float));
            m_dir              = 0.f;
            m_octave           = 0.f;
            m_level            = 0.f;
            m_o_s_coordiante.x = 0.f;
            m_o_s_coordiante.y = 0.f;
            m_coordiante.x     = 0.f;
            m_coordiante.y     = 0.f;
            m_coordiante.z     = 0.f;
            m_subCoordiante.x  = 0.f;
            m_subCoordiante.y  = 0.f;
            m_subCoordiante.z  = 0.f;
            m_response         = 0.f;
            m_extremePointType = 0;
        }

        Feature &operator=(const Feature &feat)
        {
            memcpy(m_feature, feat.m_feature, 32 * sizeof(float));
            m_dir              = feat.m_dir;
            m_octave           = feat.m_octave;
            m_level            = feat.m_level;
            m_o_s_coordiante   = feat.m_o_s_coordiante;
            m_coordiante       = feat.m_coordiante;
            m_subCoordiante    = feat.m_subCoordiante;
            m_response         = feat.m_response;
            m_extremePointType = feat.m_extremePointType;
            return *this;
        }

        float    m_feature[32];
        float    m_dir; // main dir
        float    m_octave;
        float    m_level;
        Vector2f m_o_s_coordiante;   // x,y
        Vector3f m_coordiante;       // x,y,z   (x,y) * scale
        Vector3f m_subCoordiante;    // x,y,z 
        float    m_response;
        int      m_extremePointType; // 1 big  2 little  0 default
    };

public:
    typedef std::shared_ptr<SIFT> ptr;

    // 为了满足尺度变化的连续性
    SIFT(unsigned int width, unsigned int height, unsigned int maxNum = 800, unsigned int Octave = 3, unsigned int Level = 3, float delta = 1.6f, float gaussRadius = 2);

    void Run(float *gray);

    ~SIFT();

    std::vector<Feature> m_Coordiantes;

private:
    void GaussPyramidImgs(float* gray);
    void DOGPyramidImgs();
    void FindExtremePoint();

private:
    unsigned int m_width, m_height, m_octave, m_level, m_maxNum;
    float m_delta, m_gaussRadius;
    float *m_gaussKernel;

    float *m_srcImg;

    std::vector<ImgSize> m_O_Size;
    std::vector<std::vector<float *>> m_O_S_GaussImgs; // img pyramid   Octave * (Scale + 3)

    std::vector<std::vector<float *>> m_O_S_DOGImgs;   // DOG pyramid Octave * (Scale + 2)

    std::vector<std::vector<Feature>> m_O_Coordiantes;
};

#endif
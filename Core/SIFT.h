#ifndef _SIFT_H_
#define _SIFT_H_

#include <iostream>
#include <memory>
#include <memory.h>
#include <vector>
#include "Common.h"

class SIFT{

public:
    class Feature{
    public:
        Feature(Feature& feat){
            memcpy(m_feature, feat.m_feature, 32 * sizeof(float));
            m_dir = feat.m_dir;
            m_octave = feat.m_octave;
            m_scale = feat.m_scale;
            m_o_s_coordiante = feat.m_o_s_coordiante;
            m_coordiante = feat.m_coordiante;
        }

        Feature(const Feature& feat){
            memcpy(m_feature, feat.m_feature, 32 * sizeof(float));
            m_dir = feat.m_dir;
            m_octave = feat.m_octave;
            m_scale = feat.m_scale;
            m_o_s_coordiante = feat.m_o_s_coordiante;
            m_coordiante = feat.m_coordiante;
        }

        Feature(){
            memset(m_feature, 0, 32 * sizeof(float));
            m_dir = 0.f;
            m_octave = 0.f;
            m_scale = 0.f;
            m_o_s_coordiante.x = 0.f;
            m_o_s_coordiante.y = 0.f;
            m_coordiante.x = 0.f;
            m_coordiante.y = 0.f;
        }

        Feature& operator=(const Feature& feat){
            memcpy(m_feature, feat.m_feature, 32 * sizeof(float));
            m_dir = feat.m_dir;
            m_octave = feat.m_octave;
            m_scale = feat.m_scale;
            m_o_s_coordiante = feat.m_o_s_coordiante;
            m_coordiante = feat.m_coordiante;
            return *this;
        }

        float m_feature[32];
        float m_dir;         // main dir
        float m_octave;
        float m_scale;
        Coordiante_f m_o_s_coordiante;
        Coordiante_f m_coordiante;
    };

public:
    typedef std::shared_ptr<SIFT> ptr;
    // 为了满足尺度变化的连续性
    SIFT(int width, int height, int Octave = -1, int Scale = 5, float delta = 1.6f, float gaussRadius = 2);

    void Run(float* gray);
    
    ~SIFT();

    std::vector<Feature> m_Coordiantes;
private:

    int m_width, m_height, m_octave, m_scale, m_delta, m_gaussRadius;
    float* m_gaussKernel;
    
    float* m_srcImg;

    std::vector<ImgSize> m_O_Size;
    std::vector<std::vector<float*> > m_O_S_GaussImgs;  // img pyramid   Octave * (Scale + 3)

    std::vector<std::vector<float*> > m_O_S_DOGImgs; // DOG pyramid Octave * (Scale + 2)

    std::vector<std::vector<Feature> > m_O_S_Coordiantes;

};

#endif
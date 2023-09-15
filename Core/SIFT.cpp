#include <math.h>
#include "ImageTransformation.h"
#include "ImageInterpolation.h"
#include "SIFT.h"

SIFT::SIFT(int width, int height, int Octave, int Scale, float delta, float gaussRadius)
: m_width(width), m_height(height), m_octave(Octave), m_scale(Scale), m_delta(delta), m_gaussRadius(gaussRadius){

    std::cout << "SIFT Initting" << std::endl;
    int ksize = m_gaussRadius * 2 + 1;
    m_gaussKernel = (float*)calloc(ksize * ksize, sizeof(float));

    if(m_octave == -1){
        m_octave = (int)(log2f((m_width > m_height ? m_height : m_width)) - 3);
    }
    std::cout << "Octave: " << m_octave << std::endl;

    std::cout << "GaussFilter:" << std::endl;
    Utily::GaussFilter(ksize, m_delta, m_gaussKernel);
    for(int i = 0; i < ksize; ++ i){
        for(int j = 0; j < ksize; ++ j){
            std::cout << m_gaussKernel[i * ksize + j] << ", ";
        }
        std::cout << std::endl;
    }

    // compute octave img size
    ImgSize imgSize;
    imgSize.x = m_width;
    imgSize.y = m_height;

    std::cout << "m_O_Size:" << std::endl;
    m_O_Size.push_back(imgSize);
    std::cout << "m_octave: " << m_octave << std::endl;
    for(int o = 1; o < m_octave; ++o){
        ImgSize imgSize;
        imgSize.x = (int)(m_O_Size[o - 1].x * 0.5f + 0.5f);
        imgSize.y = (int)(m_O_Size[o - 1].y * 0.5f + 0.5f);
        m_O_Size.push_back(imgSize);
    }

    for(int o = 0; o < m_octave; ++o){
        std::cout << m_O_Size[o].x << ", " << m_O_Size[o].y << std::endl;
    }

    // calloc memory
    m_srcImg = (float*)calloc(m_width * m_height, sizeof(float));

    for(int o = 0; o < m_octave; ++ o){
        std::vector<float*> s_gaussImgs;
        for(int s = 0; s < m_scale + 3; ++ s){
            float* gaussImg = (float*)calloc(m_O_Size[o].x * m_O_Size[o].y, sizeof(float));
            s_gaussImgs.push_back(gaussImg);
            std::cout << o << "  " << s << std::endl;
        }
        m_O_S_GaussImgs.push_back(s_gaussImgs);
    }

    for(int o = 0; o < m_octave; ++ o){
        std::vector<float*> s_dogImgs;
        for(int s = 0; s < m_scale + 2; ++ s){
            float* dogImg = (float*)calloc(m_O_Size[o].x * m_O_Size[o].y, sizeof(float));
            s_dogImgs.push_back(dogImg);
        }
        m_O_S_DOGImgs.push_back(s_dogImgs);
    }
    m_O_S_Coordiantes.resize(m_octave);

    std::cout << "SIFT Initted" << std::endl;
}

void SIFT::Run(float* gray){

    std::cout << "SIFT Running" << std::endl;

    // clear pre result
    for(int o = 0; o < m_octave; ++ o){
        m_O_S_Coordiantes[o].clear();
    }
    
    std::cout << "Creatting Gauss Image pyramid." << std::endl;
    // build gauss image pyramid
    for(int o = 0; o < m_octave; ++ o){

        for(int s = 0; s < m_scale + 3; ++ s){
            std::cout << "GaussImage: " << o << "   " << s << std::endl;
            if(o == 0 && s == 0){
                Utily::ImageConvolution1(gray, m_O_Size[o].x, m_O_Size[o].y, 1, m_O_S_GaussImgs[o][s], m_gaussKernel, m_gaussRadius * 2 + 1);
            }else if(o > 0 && s == 0){
                ImageInterpolation_Nearest(m_O_S_GaussImgs[o - 1][m_scale], m_O_Size[o - 1].x, m_O_Size[o - 1].y, 1, m_O_S_GaussImgs[o][s], m_O_Size[o].x, m_O_Size[o].y);
            }else{
                Utily::ImageConvolution1(m_O_S_GaussImgs[o][s - 1], m_O_Size[o].x, m_O_Size[o].y, 1, m_O_S_GaussImgs[o][s], m_gaussKernel, m_gaussRadius * 2 + 1);
            }

            std::string savePath = std::to_string(o) + "_" + std::to_string(s) + "_gaussImg.jpg";
            Utily::SaveImage<float>(savePath, m_O_Size[o].x, m_O_Size[o].y, 1, m_O_S_GaussImgs[o][s]);
        }
    }

#if 0
    {
        for(int o = 0; o < m_octave; ++ o){
            for(int s = 0; s < m_scale + 3; ++ s){
                std::string savePath = std::to_string(o) + "_" + std::to_string(s) + "_gaussImg.jpg";
                Utily::SaveImage<float>(savePath, m_O_Size[o].x, m_O_Size[o].y, 1, m_O_S_GaussImgs[o][s]);
            }
        }
    }
#endif


    // build DOG pyramid
    for(int o = 0; o < m_octave; ++ o){
        for(int s = 0; s < m_scale + 2; ++ s){
            Utily::TwoImageDiff(m_O_S_GaussImgs[o][s + 1], m_O_S_GaussImgs[o][s], m_O_Size[o].x, m_O_Size[o].y, 1, m_O_S_DOGImgs[o][s]);
        }
    }

#if 0
    {
        for(int o = 0; o < m_octave; ++ o){
            for(int s = 0; s < m_scale + 2; ++ s){
                std::string savePath = std::to_string(o) + "_" + std::to_string(s) + "_DOGImg.jpg";
                Utily::SaveImage<float>(savePath, m_O_Size[o].x, m_O_Size[o].y, 1, m_O_S_DOGImgs[o][s]);
            }
        }
    }
#endif
    m_O_S_Coordiantes.clear();
    // check extreme point
    for(int o = 0; o < m_octave; ++ o){
        for(int s = 1; s < m_scale + 1; ++ s){
            float* downImg = m_O_S_DOGImgs[o][s - 1];
            float* curImg = m_O_S_DOGImgs[o][s];
            float* upImg = m_O_S_DOGImgs[o][s + 1];

            int height = m_O_Size[o].y;
            int width = m_O_Size[o].x;

            std::vector<Feature> oFeatures;

            for(int i = 1; i < height - 1; ++ i){
                for(int j = 1; j < width - 1; ++ j){
                    float m111 = curImg[i * width + j];

                    float m000 = downImg[(i - 1) * width + j - 1];
                    float m001 = downImg[(i - 1) * width + j];
                    float m002 = downImg[(i - 1) * width + j + 1];
                    float m010 = downImg[i * width + j - 1];
                    float m011 = downImg[i * width + j];
                    float m012 = downImg[i * width + j + 1];
                    float m020 = downImg[(i + 1) * width + j - 1];
                    float m021 = downImg[(i + 1) * width + j];
                    float m022 = downImg[(i + 1) * width + j + 1];
                    
                    float m100 = curImg[(i - 1) * width + j - 1];
                    float m101 = curImg[(i - 1) * width + j];
                    float m102 = curImg[(i - 1) * width + j + 1];
                    float m110 = curImg[i * width + j - 1];
                    float m112 = curImg[i * width + j + 1];
                    float m120 = curImg[(i + 1) * width + j - 1];
                    float m121 = curImg[(i + 1) * width + j];
                    float m122 = curImg[(i + 1) * width + j + 1];

                    float m200 = upImg[(i - 1) * width + j - 1];
                    float m201 = upImg[(i - 1) * width + j];
                    float m202 = upImg[(i - 1) * width + j + 1];
                    float m210 = upImg[i * width + j - 1];
                    float m211 = upImg[i * width + j];
                    float m212 = upImg[i * width + j + 1];
                    float m220 = upImg[(i + 1) * width + j - 1];
                    float m221 = upImg[(i + 1) * width + j];
                    float m222 = upImg[(i + 1) * width + j + 1];
                    // find extreme point
                    if((m111 > m000 && m111 > m001 && m111 > m002 && m111 > m010 && m111 > m011 && m111 > m012 && m111 > m020 && m111 > m021 && m111 > m022 && 
                        m111 > m100 && m111 > m101 && m111 > m102 && m111 > m110 &&                m111 > m112 && m111 > m120 && m111 > m121 && m111 > m122 && 
                        m111 > m200 && m111 > m201 && m111 > m202 && m111 > m210 && m111 > m211 && m111 > m212 && m111 > m220 && m111 > m221 && m111 > m222) || 
                       (m111 < m000 && m111 < m001 && m111 < m002 && m111 < m010 && m111 < m011 && m111 < m012 && m111 < m020 && m111 < m021 && m111 < m022 && 
                        m111 < m100 && m111 < m101 && m111 < m102 && m111 < m110 &&                m111 < m112 && m111 < m120 && m111 < m121 && m111 < m122 && 
                        m111 < m200 && m111 < m201 && m111 < m202 && m111 < m210 && m111 < m211 && m111 < m212 && m111 < m220 && m111 < m221 && m111 < m222)){
                        Feature feature;
                        feature.m_octave = o;
                        feature.m_scale = s;
                        feature.m_o_s_coordiante.x = j;
                        feature.m_o_s_coordiante.y = i;

                        feature.Jacobi[0] = (m112 - m110) * 0.5f;
                        feature.Jacobi[1] = (m121 - m101) * 0.5f;
                        feature.Jacobi[2] = (m211 - m011) * 0.5f;

                        feature.Hession[0] = (m112 + m110) - 2.f * m111;
                        feature.Hession[1] = ((m122 - m120) - (m102 - m100)) * 0.25f;
                        feature.Hession[2] = ((m212 - m210) - (m012 - m010)) * 0.25f;

                        feature.Hession[3] = ((m122 - m102) - (m120 - m100)) * 0.25f;
                        feature.Hession[4] = (m121 + m101) - 2.f * m111;
                        feature.Hession[5] = ((m221 - m201) - (m021 - m001)) * 0.25f;

                        feature.Hession[6] = ((m212 - m012) - (m210 - m010)) * 0.25f;
                        feature.Hession[7] = ((m221 - m021) - (m201 - m001)) * 0.25f;
                        feature.Hession[8] = (m211 + m011) - 2.f * m111;


                        oFeatures.push_back(feature);
                    }
                }
            }
            m_O_S_Coordiantes.push_back(oFeatures);
        }
    }
}
    
SIFT::~SIFT(){
    FREE_Memory(m_gaussKernel);

    FREE_Memory(m_srcImg);

    for(int o = 0; o < m_octave; ++ o){        
        for(int s = 0; s < m_scale + 3; ++ s){
            FREE_Memory(m_O_S_GaussImgs[o][s]);
        }
        m_O_S_GaussImgs[o].clear();
    }
    m_O_S_GaussImgs.clear();

    for(int o = 0; o < m_octave; ++ o){
        
        for(int s = 0; s < m_scale + 2; ++ s){
            FREE_Memory(m_O_S_DOGImgs[o][s]);
        }
        m_O_S_DOGImgs[o].clear();
    }
    m_O_S_DOGImgs.clear();

    for(int o = 0; o < m_octave; ++ o){
        m_O_S_Coordiantes[o].clear();
    }
    m_O_S_Coordiantes.clear();
}
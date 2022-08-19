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

#if 1
    {
        for(int o = 0; o < m_octave; ++ o){
            for(int s = 0; s < m_scale + 2; ++ s){
                std::string savePath = std::to_string(o) + "_" + std::to_string(s) + "_DOGImg.jpg";
                Utily::SaveImage<float>(savePath, m_O_Size[o].x, m_O_Size[o].y, 1, m_O_S_DOGImgs[o][s]);
            }
        }
    }
#endif
    // 
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
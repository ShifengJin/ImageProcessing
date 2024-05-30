#include <math.h>
#include <algorithm>
#include "ImageTransformation.h"
#include "ImageInterpolation.h"
#include "SIFT.h"

SIFT::SIFT(unsigned int width, unsigned int height, unsigned int maxNum, unsigned int Octave, unsigned int Level, float sigma)
: m_width(width), m_height(height), m_maxNum(maxNum), m_octave(Octave), m_level(Level), m_sigma(sigma){

    std::cout << "SIFT Initting" << std::endl;
    
    if(m_octave == -1){
        m_octave = (unsigned int)(log2f((m_width > m_height ? m_height : m_width)) - 3);
    }
    std::cout << "Octave: " << m_octave << std::endl;
    // Gauss sigma
    float sigma0 = sqrtf(m_sigma * sigma - SIFT_INIT_SIGMA * SIFT_INIT_SIGMA);
    float k = powf(2.f, 1.f / m_level);

    std::cout << "sigma0: " << sigma0 << std::endl;
    
    for(unsigned int l = 0; l < m_level + 3; ++l){
        float sig = powf(k, l) * sigma0;
        m_L_Sigma.push_back(sig);
    }
    
    for(unsigned int l = 0; l < m_level + 3; ++ l){
        int ksize = SIFT_GAUSS_KERNEL_RATIO * m_L_Sigma[l] + 1;
        if(ksize % 2 == 0){
            ksize += 1;
        }
        GaussFilter* gaussFilteri = new GaussFilter(ksize, m_L_Sigma[l]);
        m_Level_GaussFilters.push_back(gaussFilteri);
    }

    for(unsigned int i = 0; i < m_Level_GaussFilters.size(); ++i){
        std::cout << "GaussFilter: " << m_Level_GaussFilters[i]->kernel_width << ", " << m_Level_GaussFilters[i]->sigma << std::endl;
    }
    
    // compute octave img size
    ImgSize imgSize;
    imgSize.x = m_width;
    imgSize.y = m_height;
    
    std::cout << "m_O_Size:" << std::endl;
    m_O_Size.push_back(imgSize);
    for(unsigned int o = 1; o < m_octave; ++o){
        ImgSize imgSize;
        imgSize.x = (int)(m_O_Size[o - 1].x * 0.5f + 0.5f);
        imgSize.y = (int)(m_O_Size[o - 1].y * 0.5f + 0.5f);
        m_O_Size.push_back(imgSize);
    }

    for(unsigned int o = 0; o < m_octave; ++o){
        std::cout << m_O_Size[o].x << ", " << m_O_Size[o].y << std::endl;
    }

    // calloc memory
    m_srcImg = (float*)calloc(m_width * m_height, sizeof(float));

    for(unsigned int o = 0; o < m_octave; ++ o){
        std::vector<float*> s_gaussImgs;
        for(unsigned int s = 0; s < m_level + 3; ++ s){
            float* gaussImg = (float*)calloc(m_O_Size[o].x * m_O_Size[o].y, sizeof(float));
            s_gaussImgs.push_back(gaussImg);
            std::cout << o << "  " << s << std::endl;
        }
        m_O_L_GaussImgs.push_back(s_gaussImgs);
    }

    for(unsigned int o = 0; o < m_octave; ++ o){
        std::vector<float*> s_dogImgs;
        for(unsigned int s = 0; s < m_level + 2; ++ s){
            float* dogImg = (float*)calloc(m_O_Size[o].x * m_O_Size[o].y, sizeof(float));
            s_dogImgs.push_back(dogImg);
        }
        m_O_L_DOGImgs.push_back(s_dogImgs);
    }
    
    std::cout << "SIFT Initted" << std::endl;
}

bool featureResponseCmp(SIFT::Feature& in0, SIFT::Feature& in1){
    return in0.m_response > in1.m_response;
}

void SIFT::Run(float* gray){

    std::cout << "SIFT Running" << std::endl;

    // clear pre result
    // for(int o = 0; o < m_octave; ++ o){
    //     m_O_Coordiantes[o].clear();
    // }
    
    std::cout << "Creatting Gauss Image pyramid." << std::endl;
    // build gauss image pyramid
    GaussPyramidImgs(gray);

    // build DOG pyramid
    DOGPyramidImgs();

    // m_O_Coordiantes.clear();
    // check extreme point
    FindExtremePoint();
    
    


    // choose Feature
    std::sort(m_Coordiantes.begin(), m_Coordiantes.end(), featureResponseCmp);
    if(m_Coordiantes.size() > m_maxNum){
        m_Coordiantes.resize(m_maxNum);
    }
    
    // ori and desc

}
    
void SIFT::GaussPyramidImgs(float* gray){

    for(unsigned int o = 0; o < m_octave; ++ o){

        for(unsigned int s = 0; s < m_level + 3; ++ s){
            std::cout << "GaussImage: " << o << "   " << s << std::endl;
            if(o == 0 && s == 0){                
                Utily::ImageConvolution2(gray, m_O_Size[o].x, m_O_Size[o].y, m_O_L_GaussImgs[o][s], m_Level_GaussFilters[s]->kernel, m_Level_GaussFilters[s]->kernel_width);
            }else if(o > 0 && s == 0){
                ImageInterpolation_Nearest(m_O_L_GaussImgs[o - 1][m_level], m_O_Size[o - 1].x, m_O_Size[o - 1].y, 1, m_O_L_GaussImgs[o][s], m_O_Size[o].x, m_O_Size[o].y);
            }else{

                Utily::ImageConvolution2(m_O_L_GaussImgs[o][0], m_O_Size[o].x, m_O_Size[o].y, m_O_L_GaussImgs[o][s], m_Level_GaussFilters[s]->kernel, m_Level_GaussFilters[s]->kernel_width);
            }
        }
    }

#if 0
    {
        for(int o = 0; o < m_octave; ++ o){
            for(int s = 0; s < m_level + 3; ++ s){
                std::string savePath = std::to_string(o) + "_" + std::to_string(s) + "_gaussImg.jpg";
                Utily::SaveImage<float>(savePath, m_O_Size[o].x, m_O_Size[o].y, 1, m_O_L_GaussImgs[o][s]);
                // std::string savePath = std::to_string(o) + "_" + std::to_string(s) + "_gaussImg.txt";
                // std::string savePath = std::to_string(o) + "_" + std::to_string(s) + "_gaussImg.txt";
                // Utily::SaveBuffer(savePath, m_O_Size[o].x, m_O_Size[o].y, 1, m_O_L_GaussImgs[o][s]);
            }
        }
    }
#endif

}

void SIFT::DOGPyramidImgs(){
    for(unsigned int o = 0; o < m_octave; ++ o){
        for(unsigned int s = 0; s < m_level + 2; ++ s){
            Utily::TwoImageDiff(m_O_L_GaussImgs[o][s + 1], m_O_L_GaussImgs[o][s], m_O_Size[o].x, m_O_Size[o].y, 1, m_O_L_DOGImgs[o][s]);
        }
    }

#if 0
    {
        for(int o = 0; o < m_octave; ++ o){
            for(int s = 0; s < m_level + 2; ++ s){
                std::string savePath = std::to_string(o) + "_" + std::to_string(s) + "_DOGImg.jpg";
                Utily::SaveImage<float>(savePath, m_O_Size[o].x, m_O_Size[o].y, 1, m_O_L_DOGImgs[o][s]);
            }
        }
    }
#endif
}



void SIFT::FindExtremePoint(){

#if SHOWEXTREMEPOINTLOG
    findNum  = 0;
    NGPixelThresholdNum = 0;
    extremeNum = 0;
    bigExtremeNum = 0;
    littleExtremeNum = 0;

    outBorderNum = 0;
    DetHInvWroundNum = 0;
    xOffsetOutNum = 0;
    yOffsetOutNum = 0;
    sOffsetOutNum = 0;
    iterOutNum = 0;
    responseOutNum = 0;
    detHOutNum = 0;
    edgeOutNum = 0;
#endif


    float scaleOffset[10] = { 0.f, 0.5f, 1.5f, 3.5f, 7.5f, 15.5f, 31.5f, 63.5f, 127.5f, 255.5f };
    float octaveScale[10] = { 1.f, 2.f, 4.f, 8.f, 16.f, 32.f, 64.f, 128.f, 256.f, 512.f };
    float SubZThreshold = m_octave * m_level + 1.f;

    m_Coordiantes.clear();

    for(unsigned int o = 0; o < m_octave; ++ o){
        
        float coordinateScale = octaveScale[o];
        float coordinateOffset = scaleOffset[o];
        
        for(unsigned int s = 1; s < m_level + 1; ++ s){

            float pyramidLevel = o * m_level + s;

            float* downImg = m_O_L_DOGImgs[o][s - 1];
            float* curImg  = m_O_L_DOGImgs[o][s];
            float* upImg   = m_O_L_DOGImgs[o][s + 1];

            unsigned int height = m_O_Size[o].y;
            unsigned int width  = m_O_Size[o].x;

            std::cout << "Oct: " << o << "    level: " << s << "    width: " << width << "    height: " << height << std::endl;
            for(unsigned int i = SIFT_IMG_BORDER; i < height - SIFT_IMG_BORDER; ++ i){
                int preRow = (i - 1) * width;
                int curRow = i * width;
                int nxtRow = (i + 1) * width;
                for(unsigned int j = SIFT_IMG_BORDER; j < width - SIFT_IMG_BORDER; ++ j){
                    
#if SHOWEXTREMEPOINTLOG
                    findNum ++;
#endif

                    float A22 = curImg[curRow + j];
                    if(fabs(A22) < SIFT_EXTREMA_TH_PIXEL){
#if SHOWEXTREMEPOINTLOG
                        NGPixelThresholdNum ++;
#endif
                        continue;
                    }

                    float B11 = downImg[preRow + j - 1];
                    float B12 = downImg[preRow + j];
                    float B13 = downImg[preRow + j + 1];

                    float B21 = downImg[curRow + j - 1];
                    float B22 = downImg[curRow + j];
                    float B23 = downImg[curRow + j + 1];

                    float B31 = downImg[nxtRow + j - 1];
                    float B32 = downImg[nxtRow + j];
                    float B33 = downImg[nxtRow + j + 1];
                    

                    float A11 = curImg[preRow + j - 1];
                    float A12 = curImg[preRow + j];
                    float A13 = curImg[preRow + j + 1];

                    float A21 = curImg[curRow + j - 1];
                    float A23 = curImg[curRow + j + 1];

                    float A31 = curImg[nxtRow + j - 1];
                    float A32 = curImg[nxtRow + j];
                    float A33 = curImg[nxtRow + j + 1];


                    float C11 = upImg[preRow + j - 1];
                    float C12 = upImg[preRow + j];
                    float C13 = upImg[preRow + j + 1];

                    float C21 = upImg[curRow + j - 1];
                    float C22 = upImg[curRow + j];
                    float C23 = upImg[curRow + j + 1];

                    float C31 = upImg[nxtRow + j - 1];
                    float C32 = upImg[nxtRow + j];
                    float C33 = upImg[nxtRow + j + 1];

                    int extremePointType = 0;
                    // find extreme point
                    if( A22 > B11 && A22 > B12 && A22 > B13 && A22 > B21 && A22 > B22 && A22 > B23 && A22 > B31 && A22 > B32 && A22 > B33 && 
                        A22 > A11 && A22 > A12 && A22 > A13 && A22 > A21 &&              A22 > A23 && A22 > A31 && A22 > A32 && A22 > A33 && 
                        A22 > C11 && A22 > C12 && A22 > C13 && A22 > C21 && A22 > C22 && A22 > C23 && A22 > C31 && A22 > C32 && A22 > C33)
                    {
#if SHOWEXTREMEPOINTLOG
                        bigExtremeNum ++;
#endif
                        extremePointType = 1;
                    }
                    else if (A22 < B11 && A22 < B12 && A22 < B13 && A22 < B21 && A22 < B22 && A22 < B23 && A22 < B31 && A22 < B32 && A22 < B33 && 
                             A22 < A11 && A22 < A12 && A22 < A13 && A22 < A21 &&              A22 < A23 && A22 < A31 && A22 < A32 && A22 < A33 && 
                             A22 < C11 && A22 < C12 && A22 < C13 && A22 < C21 && A22 < C22 && A22 < C23 && A22 < C31 && A22 < C32 && A22 < C33)
                    {
#if SHOWEXTREMEPOINTLOG
                        littleExtremeNum ++;
#endif
                        extremePointType = 2;
                    }else
                    {
                        continue;
                    }

#if SHOWEXTREMEPOINTLOG
                    extremeNum ++;
#endif


                    Feature feature;
                    bool ret = AdjustLocalExtrema(width, height, j, i, o, s, extremePointType, coordinateScale, coordinateOffset, feature);
                    if(ret){
                        m_Coordiantes.push_back(feature);
                    }
                }
            }
        }
    }

#if SHOWEXTREMEPOINTLOG
    std::cout << "findNUm: " << findNum << std::endl;
    std::cout << "NGPixelThresholdNum: " << NGPixelThresholdNum << std::endl;
    std::cout << "extremeNum: " << extremeNum << std::endl;
    std::cout << "bigExtremeNum: " << bigExtremeNum << std::endl;
    std::cout << "littleExtremeNum: " << littleExtremeNum << std::endl;
    
    std::cout << "outBorderNum: " << outBorderNum  << std::endl;
    std::cout << "DetHInvWroundNum: " << DetHInvWroundNum  << std::endl;
    std::cout << "xOffsetOutNum: " << xOffsetOutNum  << std::endl;
    std::cout << "yOffsetOutNum: " << yOffsetOutNum  << std::endl;
    std::cout << "sOffsetOutNum: " << sOffsetOutNum  << std::endl;
    std::cout << "iterOutNum: " << iterOutNum  << std::endl;
    std::cout << "responseOutNum: " << responseOutNum  << std::endl;
    std::cout << "detHOutNum: " << detHOutNum  << std::endl;
    std::cout << "edgeOutNum: " << edgeOutNum  << std::endl;
#endif


}

bool SIFT::AdjustLocalExtrema(int width, int height, int x, int y, int o, int l, int extremePointType, float scale, float offset, Feature& oFeature){

    float B11 = 0.f, B12 = 0.f, B13 = 0.f, B21 = 0.f, B22 = 0.f, B23 = 0.f, B31 = 0.f, B32 = 0.f, B33 = 0.f;            
    float A11 = 0.f, A12 = 0.f, A13 = 0.f, A21 = 0.f, A22 = 0.f, A23 = 0.f, A31 = 0.f, A32 = 0.f, A33 = 0.f;
    float C11 = 0.f, C12 = 0.f, C13 = 0.f, C21 = 0.f, C22 = 0.f, C23 = 0.f, C31 = 0.f, C32 = 0.f, C33 = 0.f;

    float Dy = 0.f, Dx = 0.f, DS = 0.f;

    float *downImg = NULL, *curImg = NULL, *upImg = NULL;

    float Dxx = 0.f, Dyy = 0.f, DSS = 0.f, Dxy = 0.f, DSX = 0.f, DSY = 0.f;

    float xyOffsetTh = 4.f / (o + 1);
    
    float xOffset = 0.f, yOffset = 0.f, sOffset = 0.f;

    int subX = x;
    int subY = y;
    int subS = l;

    int iterCount = 0;
    while (iterCount < SIFT_MAX_INTERP_STEPS){
        if(subX < SIFT_IMG_BORDER || subX >= width - SIFT_IMG_BORDER ||
           subY < SIFT_IMG_BORDER || subY >= height - SIFT_IMG_BORDER ||
           subS < 1 || subS > m_level)
        {
#if SHOWEXTREMEPOINTLOG
            outBorderNum ++;
#endif
            return false;
        }

        downImg = m_O_L_DOGImgs[o][subS - 1];
        curImg  = m_O_L_DOGImgs[o][subS];
        upImg   = m_O_L_DOGImgs[o][subS + 1];

        int preRow = (subY - 1) * width;
        int curRow = subY * width;
        int nxtRow = (subY + 1) * width;

        B11 = downImg[preRow + subX - 1];
        B12 = downImg[preRow + subX];
        B13 = downImg[preRow + subX + 1];
        B21 = downImg[curRow + subX - 1];
        B22 = downImg[curRow + subX];
        B23 = downImg[curRow + subX + 1];
        B31 = downImg[nxtRow + subX - 1];
        B32 = downImg[nxtRow + subX];
        B33 = downImg[nxtRow + subX + 1];
    
        A11 = curImg[preRow + subX - 1];
        A12 = curImg[preRow + subX];
        A13 = curImg[preRow + subX + 1];
        A21 = curImg[curRow + subX - 1];
        A22 = curImg[curRow + subX];
        A23 = curImg[curRow + subX + 1];
        A31 = curImg[nxtRow + subX - 1];
        A32 = curImg[nxtRow + subX];
        A33 = curImg[nxtRow + subX + 1];

        C11 = upImg[preRow + subX - 1];
        C12 = upImg[preRow + subX];
        C13 = upImg[preRow + subX + 1];
        C21 = upImg[curRow + subX - 1];
        C22 = upImg[curRow + subX];
        C23 = upImg[curRow + subX + 1];
        C31 = upImg[nxtRow + subX - 1];
        C32 = upImg[nxtRow + subX];
        C33 = upImg[nxtRow + subX + 1];


        Dy = (A32 - A12) * 0.5f;
        Dx = (A23 - A21) * 0.5f;
        DS = (C22 - B22) * 0.5f;
        

        //     | Dxx    Dxy    DSx |
        // H = | Dyx    Dyy    DSy |
        //     | DSx    DSy    DSS |
        Dxx = A21 + A23 - 2.f * A22;
        Dyy = A12 + A32 - 2.f * A22;
        DSS = C22 + B22 - 2.f * A22;
        Dxy = (A33 + A11 - (A13 + A31)) * 0.25f;
        DSX = (B21 + C23 - (B23 + C21)) * 0.25f;
        DSY = (B12 + C32 - (B32 + C12)) * 0.25f;

        float detH = Dxx * Dyy * DSS + Dxy * DSY * DSX + DSX * Dxy * DSY - (Dxx * DSY * DSY + Dxy * Dxy * DSS + DSX * Dyy * DSX);
        if(fabsf(detH) < FLT_EPSILON){

#if SHOWEXTREMEPOINTLOG
            DetHInvWroundNum ++;
#endif

            return false;
        }

        detH = 1.f / detH;
    
        float MInv_11 = Dyy * DSS - DSY * DSY;
        float MInv_12 = DSX * DSY - Dxy * DSS;
        float MInv_13 = Dxy * DSY - Dyy * DSX;

        xOffset = (MInv_11 * -Dx + MInv_12 * -Dy + MInv_13 * -DS) * detH;
        if(fabsf(xOffset) > xyOffsetTh){
#if SHOWEXTREMEPOINTLOG
            xOffsetOutNum ++;
#endif
            return false;
        }


        float MInv_21 = DSX * DSY - Dxy * DSS;
        float MInv_22 = Dxx * DSS - DSX * DSX;
        float MInv_23 = Dxy * DSX - Dxx * DSY;
        yOffset = (MInv_21 * -Dx + MInv_22 * -Dy + MInv_23 * -DS) * detH;
        if(fabsf(yOffset) > xyOffsetTh){
#if SHOWEXTREMEPOINTLOG
            yOffsetOutNum ++;
#endif
            return false;
        }

        float MInv_31 = Dxy * DSY - Dyy * DSX;
        float MInv_32 = Dxy * DSX - Dxx * DSY;
        float MInv_33 = Dxx * Dyy - Dxy * Dxy;
        sOffset = (MInv_31 * (-Dx) + MInv_32 * (-Dy) + MInv_33 * (-DS)) * detH;
        if(fabsf(sOffset) > 1.f){
#if SHOWEXTREMEPOINTLOG
            sOffsetOutNum ++;
#endif
            return false;
        }
        
        if(fabs(xOffset) < 0.5f && fabs(yOffset) < 0.5f && fabs(sOffset) < 0.5f){
            break;
        }

        subX += (int)roundf(xOffset);
        subY += (int)roundf(yOffset);
        subS += (int)roundf(sOffset);

        iterCount++;
    }

    if(iterCount >= SIFT_MAX_INTERP_STEPS){
#if SHOWEXTREMEPOINTLOG
        iterOutNum ++;
#endif
        return false;
    }

    float AnsSubX = subX + xOffset;
    float AnsSubY = subY + yOffset;
    float AnsSubS = subS + sOffset;

    if(AnsSubX < SIFT_IMG_BORDER || AnsSubX >= width - SIFT_IMG_BORDER ||
       AnsSubY < SIFT_IMG_BORDER || AnsSubY >= height - SIFT_IMG_BORDER ||
       AnsSubS < 1 || AnsSubS > m_level + 1)
    {
#if SHOWEXTREMEPOINTLOG
        std::cout << AnsSubX << ", " << AnsSubY << ", " << AnsSubS << std::endl;
        outBorderNum ++;
#endif
        return false;
    }
    
    float response = fabsf(A22 + (Dx * xOffset + Dy * yOffset + DS * sOffset) * 0.5f);
    if(response < SIFT_RESPONSE_TH)
    {
#if SHOWEXTREMEPOINTLOG
        responseOutNum ++;
#endif
        return false;
    }
    
    float detH = Dxx * Dyy - Dxy * Dxy;
    if(detH <= FLT_EPSILON){
#if SHOWEXTREMEPOINTLOG
        detHOutNum ++;
#endif
        return false;
    }

    float TrH = Dxx + Dyy;
    float r_1_2__r_2 = (TrH * TrH) / detH;
    if(fabsf(r_1_2__r_2) > SIFT_EDGERESPONSE){
#if SHOWEXTREMEPOINTLOG
        edgeOutNum ++;
#endif
        return false;
    }

    float subCoordX = AnsSubX * scale + offset;
    float subCoordY = AnsSubY * scale + offset;
    float subLevel = o * m_level + AnsSubS;
    
    oFeature.m_octave = o;
    oFeature.m_level = l;
    oFeature.m_subLevel = AnsSubS;
    oFeature.m_o_s_coordiante.x = x;
    oFeature.m_o_s_coordiante.y = y;
    oFeature.m_o_s_subCoordiante.x = AnsSubX;
    oFeature.m_o_s_subCoordiante.y = subCoordY;

    oFeature.m_pyramidLevel = subLevel;
    oFeature.m_pyramidScale = m_sigma * powf(2.f, subLevel / m_level);

    oFeature.m_octScale = m_sigma * powf(2.f, oFeature.m_subLevel / m_level);

    oFeature.m_coordiante.x = x + scale + offset;
    oFeature.m_coordiante.y = y + scale + offset;

    oFeature.m_response = response;
    oFeature.m_extremePointType = extremePointType;
    oFeature.m_subCoordiante.x = subCoordX;
    oFeature.m_subCoordiante.y = subCoordY;        
    oFeature.m_dir = 0.f;
    return true;
}

SIFT::~SIFT(){
    
    FREE_Memory(m_srcImg);

    for(int o = 0; o < m_octave; ++ o){        
        for(int s = 0; s < m_level + 3; ++ s){
            FREE_Memory(m_O_L_GaussImgs[o][s]);
        }
        m_O_L_GaussImgs[o].clear();
    }
    m_O_L_GaussImgs.clear();

    for(int o = 0; o < m_octave; ++ o){
        
        for(int s = 0; s < m_level + 2; ++ s){
            FREE_Memory(m_O_L_DOGImgs[o][s]);
        }
        m_O_L_DOGImgs[o].clear();
    }
    m_O_L_DOGImgs.clear();

    for(unsigned int i = 0; i < m_Level_GaussFilters.size(); ++ i){
        delete m_Level_GaussFilters[i];
        m_Level_GaussFilters[i] = NULL;
    }
    m_Level_GaussFilters.clear();
}

SIFT::GaussFilter::GaussFilter(int iKernelWidth, float iSigma) : kernel_width(iKernelWidth), sigma(iSigma)
{
    kernel = new float[kernel_width * kernel_width];
    Utily::GaussFilter(kernel_width, iSigma, kernel);
}

SIFT::GaussFilter::~GaussFilter(){
    if(kernel){
        delete[] kernel;
        kernel = NULL;
    }
}